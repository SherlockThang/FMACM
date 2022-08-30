// ****************************************************************************
// NOTICE
//
// This work was produced for the U.S. Government under Contract 693KA8-22-C-00001 
// and is subject to Federal Aviation Administration Acquisition Management System 
// Clause 3.5-13, Rights In Data-General, Alt. III and Alt. IV (Oct. 1996).
//
// The contents of this document reflect the views of the author and The MITRE 
// Corporation and do not necessarily reflect the views of the Federal Aviation 
// Administration (FAA) or the Department of Transportation (DOT). Neither the FAA 
// nor the DOT makes any warranty or guarantee, expressed or implied, concerning 
// the content or accuracy of these views.
//
// For further information, please contact The MITRE Corporation, Contracts Management 
// Office, 7515 Colshire Drive, McLean, VA 22102-7539, (703) 983-6000.
//
// 2022 The MITRE Corporation. All Rights Reserved.
// ****************************************************************************

#include "public/CoreUtils.h"
#include "public/AircraftControl.h"
#include "public/Environment.h"
#include "public/BadaUtils.h"

using namespace aaesim::open_source;

log4cplus::Logger AircraftControl::m_logger = log4cplus::Logger::getInstance("AircraftControl");

AircraftControl::AircraftControl()
      : m_speed_brake_gain(0.0),
        m_bada_calculator(nullptr) {
}

void AircraftControl::Initialize(std::shared_ptr<aaesim::BadaPerformanceCalculator> aircraft_performance,
                                 const Units::Angle &max_bank_angle) {
   m_max_bank_angle = max_bank_angle;
   m_bada_calculator = aircraft_performance;
   m_ac_mass = m_bada_calculator->GetAircraftMass();
   m_wing_area = m_bada_calculator->GetAerodynamicsInformation().S;
   m_is_level_flight = true;
}

Units::Frequency AircraftControl::CalculateThrustGain() {
   const double zeta = 0.88;
   m_natural_frequency = Units::HertzFrequency(0.20);
   const Units::Frequency thrustGain = 2 * zeta * m_natural_frequency;
   return thrustGain;
}

void AircraftControl::ConfigureFlapsAndEstimateKineticForces(const EquationsOfMotionState &equations_of_motion_state,
                                                             Units::Force &lift,
                                                             Units::Force &drag,
                                                             aaesim::open_source::bada_utils::FlapConfiguration &new_flap_configuration,
                                                             const WeatherTruth &weather) {
   Units::Speed calibrated_airspeed = weather.getAtmosphere()->TAS2CAS(Units::MetersPerSecondSpeed(equations_of_motion_state.true_airspeed), Units::MetersLength(
      equations_of_motion_state.altitude_msl));

   // Get temp, density, and pressure
   Units::KilogramsMeterDensity rho;
   Units::Pressure pressure;
   weather.getAtmosphere()->AirDensity(equations_of_motion_state.altitude_msl, rho, pressure);

   // Get AC Configuration
   double cd0, cd2;
   double gear;
   m_bada_calculator->GetDragCoefficientsAndIncrementFlapConfiguration(calibrated_airspeed,
                                                                       equations_of_motion_state.altitude_msl,
                                                                       cd0,
                                                                       cd2,
                                                                       gear,
                                                                       new_flap_configuration);
   LOG4CPLUS_TRACE(m_logger, "calibrated_airspeed," << Units::KnotsSpeed(calibrated_airspeed) << ","
                             "equations_of_motion_state.altitude_msl," << Units::FeetLength(equations_of_motion_state.altitude_msl) << ","
                             "new_flap_configuration," << GetFlapConfigurationAsString(new_flap_configuration));

   // Lift and Drag Estimate Calculations
   double cL =
         (2. * m_ac_mass * Units::ONE_G_ACCELERATION) / (rho * Units::sqr(equations_of_motion_state.true_airspeed) * m_wing_area * cos(equations_of_motion_state.phi));
   double cD = cd0 + gear + cd2 * pow(cL, 2);
   if (equations_of_motion_state.speedBrake != 0.0) {
      cD = (1.0 + 0.6 * equations_of_motion_state.speedBrake) * cD;
   }

   drag = 1. / 2. * rho * cD * Units::sqr(equations_of_motion_state.true_airspeed) * m_wing_area;
   lift = 1. / 2. * rho * cL * Units::sqr(equations_of_motion_state.true_airspeed) * m_wing_area;
   LOG4CPLUS_TRACE(m_logger, "drag_newtons," << Units::NewtonsForce(drag) << ","
                             "lift_newtons," << Units::NewtonsForce (lift));
}

/**
 * This default implementation of CalculateSensedWind provides "perfect" knowledge by
 * calculating and providing the true environmental winds.
 *
 * Override this method to provide alternate implemenations.
 */
void AircraftControl::CalculateSensedWind(const WeatherTruth &wind,
                                          const Units::MetersLength &altitude_msl) {
   // Get Winds and Wind Gradients at altitude_msl
   wind.getAtmosphere()->CalculateWindGradientAtAltitude(altitude_msl, wind.east_west, m_Vwx, m_dVwx_dh);
   wind.getAtmosphere()->CalculateWindGradientAtAltitude(altitude_msl, wind.north_south, m_Vwy, m_dVwy_dh);

}

Units::Angle AircraftControl::DoLateralControl(const Guidance &guidance,
                                               const EquationsOfMotionState &equations_of_motion_state) {
   const Units::InvertedLength k_xtrk = Units::PerMeterInvertedLength(5e-4);  // meters^-1
   const double k_trk = 3;      // unitless

   // States
   const Units::Speed tas = equations_of_motion_state.true_airspeed;    // true airspeed
   const Units::Angle gamma = equations_of_motion_state.gamma;        // flight-path angle
   const Units::Angle psi = equations_of_motion_state.psi_enu;        // heading angle measured from east counter-clockwise

   const Units::Speed Vw_para = m_Vwx * cos(guidance.m_enu_track_angle) + m_Vwy * sin(guidance.m_enu_track_angle);
   const Units::Speed Vw_perp = -m_Vwx * sin(guidance.m_enu_track_angle) + m_Vwy * cos(guidance.m_enu_track_angle);

   const Units::Speed wind_magnitude = sqrt(Units::sqr(m_Vwx) + Units::sqr(m_Vwy));
   const Units::Speed estimated_ground_speed = sqrt(Units::sqr(tas * cos(gamma)) - Units::sqr(Vw_perp)) + Vw_para;

   double temp = (Units::sqr(tas * cos(gamma)) + Units::sqr(estimated_ground_speed) - Units::sqr(wind_magnitude)) / (tas * 2 * cos(gamma) * estimated_ground_speed);

   // Limit temp so acos function doesn't give undefined value.
   if (temp > 1.0) {
      temp = 1.0;
   } else if (temp < -1.0) {
      temp = -1.0;
   }

   const Units::Angle beta =
         Units::RadiansAngle(acos(temp)) * -1.0 * CoreUtils::SignOfValue(Units::MetersPerSecondSpeed(Vw_perp).value());

   // Convert track guidance to heading using winds (beta is the Wind Correction Angle)
   Units::Angle heading_command = guidance.m_enu_track_angle + beta;

   // Error in heading angle
   Units::SignedAngle e_trk = heading_command - psi;
   e_trk.normalize();

   // Along-path distance and Cross-track Error
   Units::Length e_xtrk = Units::zero();

   // check if guidance has cross track error and use it if so
   if (guidance.m_use_cross_track) {
      if (guidance.m_reference_bank_angle != Units::ZERO_ANGLE) {
         e_xtrk = guidance.m_cross_track_error - (guidance.m_reference_bank_angle / k_xtrk);
      } else {
         e_xtrk = guidance.m_cross_track_error;
      }
   }

   // Calculate commanded roll angle
   // We had to add a conversion from unitless to radians in the formula for roll_angle_command.
   Units::Angle roll_angle_command = -k_xtrk * e_xtrk * Units::ONE_RADIAN_ANGLE - k_trk * e_trk;
   const double unlimited_roll_angle_command = Units::RadiansAngle(roll_angle_command).value();

   // Limit the commanded roll angle
   double sign_roll_command = CoreUtils::SignOfValue(unlimited_roll_angle_command);
   if (roll_angle_command * sign_roll_command > m_max_bank_angle) {
      roll_angle_command = m_max_bank_angle * sign_roll_command;
   }

   LOG4CPLUS_TRACE(m_logger, "cross_track_error_ft," << Units::FeetLength(e_xtrk) << ","
                             "track_angle_error_deg," << Units::DegreesAngle(e_trk) << ","
                             "roll_command_deg," << Units::DegreesAngle(roll_angle_command));

   return roll_angle_command;
}

ControlCommands AircraftControl::CalculateControlCommands(const Guidance &guidance,
                                                          const EquationsOfMotionState &equations_of_motion_state,
                                                          const WeatherTruth &truth_wind) {
   // Update environmental wind
   CalculateSensedWind(truth_wind, Units::MetersLength(equations_of_motion_state.altitude_msl));

   Units::Angle phi_command;
   aaesim::open_source::bada_utils::FlapConfiguration new_flap_config;
   double speed_brake_command = 0.0;
   Units::Force thrust_command;
   Units::Angle gamma_command;
   Units::Speed true_airspeed_command;
   if (guidance.m_active_guidance_phase==CLIMB) {
      phi_command = DoLateralControl(guidance, equations_of_motion_state);
      DoClimbingControl(guidance,
                        equations_of_motion_state,
                        thrust_command,
                        gamma_command,
                        true_airspeed_command,
                        new_flap_config,
                        truth_wind);
   } else if (guidance.m_active_guidance_phase==CRUISE_DESCENT) {
      phi_command = DoLateralControl(guidance, equations_of_motion_state);
      DoVerticalControl(guidance,
                        equations_of_motion_state,
                        thrust_command,
                        gamma_command,
                        true_airspeed_command,
                        speed_brake_command,
                        new_flap_config,
                        truth_wind);
   } else if (guidance.m_active_guidance_phase==TAKEOFF_ROLL) {
      phi_command = Units::zero();
      thrust_command = m_bada_calculator->GetMaxThrust(equations_of_motion_state.altitude_msl,
                                                       bada_utils::FlapConfiguration::TAKEOFF,
                                                       bada_utils::EngineThrustMode::MAXIMUM_CLIMB,
                                                       Units::ZERO_CELSIUS);
      gamma_command = Units::zero();
      speed_brake_command = 0;
      new_flap_config = bada_utils::TAKEOFF;
      true_airspeed_command = truth_wind.CAS2TAS(guidance.m_ias_command, equations_of_motion_state.altitude_msl);
   } else {
      throw std::runtime_error("Design Error: Missing phase of flight implementation");
   }


   ControlCommands controlCommands(phi_command, thrust_command, gamma_command, true_airspeed_command, speed_brake_command, new_flap_config);
   return controlCommands;
}

void AircraftControl::DoClimbingControl(const Guidance &guidance,
                        const EquationsOfMotionState &equations_of_motion_state,
                        Units::Force &thrust_command,
                        Units::Angle &gamma_command,
                        Units::Speed &tas_command,
                        aaesim::open_source::bada_utils::FlapConfiguration &new_flap_configuration,
                        const WeatherTruth &weather_truth) {

   const Units::Frequency gain_altitude = Units::HertzFrequency(0.20);
   const Units::Frequency thrust_gain = CalculateThrustGain();
   const Units::Frequency velocity_gain = Units::sqr(m_natural_frequency) / thrust_gain;

   const Units::Speed hdot_ref = guidance.m_vertical_speed;
   const Units::Length alt_ref = guidance.m_reference_altitude;
   const Units::Length error_alt = alt_ref - equations_of_motion_state.altitude_msl;

   double temp_gamma = -(hdot_ref + gain_altitude * error_alt) / equations_of_motion_state.true_airspeed; // calculate change in altitude
   // if fabs(change) > 1 set to 1 required for asin calculation
   if (temp_gamma > 1.0) {
      temp_gamma = 1.0;
   } else if (temp_gamma < -1.0) {
      temp_gamma = -1.0;
   }
   gamma_command = Units::RadiansAngle(asin(temp_gamma));

   // Speed Control
   tas_command = weather_truth.CAS2TAS(guidance.m_ias_command, equations_of_motion_state.altitude_msl);

   // Speed Error
   Units::Speed error_tas = tas_command - equations_of_motion_state.true_airspeed;
   Units::Acceleration vel_dot_com = velocity_gain * error_tas;

   // Estimate kinetic forces for this state
   Units::Force lift, drag;
   ConfigureFlapsAndEstimateKineticForces(equations_of_motion_state, lift, drag, new_flap_configuration, weather_truth);

   // Thrust to maintain speed
   // Nominal Thrust (no acceleration) at desired speed
   Units::Force thrust_nominal;
   thrust_nominal = m_ac_mass * vel_dot_com
      + drag - m_ac_mass * Units::ONE_G_ACCELERATION * sin(equations_of_motion_state.gamma)
      - m_ac_mass * equations_of_motion_state.true_airspeed
         * (m_dVwx_dh * cos(equations_of_motion_state.psi_enu) + m_dVwy_dh * sin(equations_of_motion_state.psi_enu))
         * sin(equations_of_motion_state.gamma) * cos(equations_of_motion_state.gamma);

   thrust_command = thrust_nominal;

   // Thrust Limits
   Units::Force max_thrust = Units::NewtonsForce(
      m_bada_calculator->GetMaxThrust(equations_of_motion_state.altitude_msl, new_flap_configuration, aaesim::open_source::bada_utils::EngineThrustMode::MAXIMUM_CLIMB, Units::ZERO_CELSIUS));
   Units::Force min_thrust = Units::NewtonsForce(
      m_bada_calculator->GetMaxThrust(equations_of_motion_state.altitude_msl, new_flap_configuration, aaesim::open_source::bada_utils::EngineThrustMode::DESCENT, Units::ZERO_CELSIUS));

   LOG4CPLUS_TRACE(m_logger, "altitude_error," << Units::FeetLength(error_alt) << "," <<
                             "thrust_command," << Units::NewtonsForce(thrust_command) << "," <<
                             "dynamics_thrust," << Units::NewtonsForce(equations_of_motion_state.thrust) << "," <<
                             "max_thrust," << Units::NewtonsForce(max_thrust) << "," <<
                             "min_thrust," << Units::NewtonsForce(min_thrust) << "," <<
                             "new_flap_configuration," << bada_utils::GetFlapConfigurationAsString(new_flap_configuration) << "," <<
                             "true_airspeed_error," << Units::KnotsSpeed(error_tas) << "," <<
                             "true_airspeed_command," << Units::KnotsSpeed(tas_command) << "," <<
                             "gamma_command," << Units::DegreesAngle(gamma_command) );
}

