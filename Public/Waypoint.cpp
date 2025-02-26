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

#include <public/AircraftIntent.h>
#include "public/Waypoint.h"

const Units::FeetLength Waypoint::MAX_ALTITUDE_CONSTRAINT(50000);
const Units::FeetLength Waypoint::MIN_ALTITUDE_CONSTRAINT(0);
const Units::FeetLength Waypoint::UNDEFINED_ALTITUDE_CONSTRAINT(-10000);
const Units::KnotsSpeed Waypoint::MAX_SPEED_CONSTRAINT(1000);
const Units::KnotsSpeed Waypoint::MIN_SPEED_CONSTRAINT(0);
const Units::KnotsSpeed Waypoint::UNDEFINED_SPEED_CONSTRAINT(-999);

Waypoint::Waypoint() {
   m_name = "";
   m_arinc424_leg_type = "";
   m_latitude = Units::ZERO_ANGLE;
   m_longitude = Units::ZERO_ANGLE;
   m_altitude = Units::ZERO_LENGTH;
   m_nominal_ias = Units::ZERO_SPEED;
   m_mach = 0;
   m_altitude_constraint_high = Units::ZERO_LENGTH;
   m_altitude_constraint_low = Units::ZERO_LENGTH;
   m_speed_constraint_high = Units::ZERO_SPEED;
   m_speed_constraint_low = Units::ZERO_SPEED;
   m_rf_turn_center_latitude = Units::ZERO_ANGLE;
   m_rf_turn_center_longitude = Units::ZERO_ANGLE;
   m_rf_turn_arc_radius = Units::ZERO_LENGTH;
}

Waypoint::Waypoint(const std::string &name,
                   Units::Angle latitude,
                   Units::Angle longitude,
                   Units::Length altitude_constraint_upper,
                   Units::Length altitude_constraint_lower,
                   Units::Speed speed_constraint,
                   Units::Length nominal_altitude,
                   Units::Speed nominal_ias,
                   std::string arinc424_leg_type) {
   m_name = name;
   m_arinc424_leg_type = arinc424_leg_type;
   m_latitude = latitude;
   m_longitude = longitude;
   m_altitude = nominal_altitude;
   m_nominal_ias = nominal_ias;

   if (std::isnan(Units::MetersLength(altitude_constraint_upper).value())) {
      altitude_constraint_upper = UNDEFINED_ALTITUDE_CONSTRAINT;
   }
   if (std::isnan(Units::MetersLength(altitude_constraint_lower).value())) {
      altitude_constraint_lower = UNDEFINED_ALTITUDE_CONSTRAINT;
   }
   if (std::isnan(Units::KnotsSpeed(speed_constraint).value())) {
      speed_constraint = UNDEFINED_SPEED_CONSTRAINT;
   }

   ProcessAltitudeConstraints(altitude_constraint_upper,
                              altitude_constraint_lower);

   ProcessSpeedConstraints(speed_constraint);

   m_rf_turn_center_latitude = Units::ZERO_ANGLE;
   m_rf_turn_center_longitude = Units::ZERO_ANGLE;
   m_rf_turn_arc_radius = Units::ZERO_LENGTH;
   m_mach = 0;
}

void Waypoint::ProcessAltitudeConstraints(Units::Length altitude_upper,
                                          Units::Length altitude_lower) {
   if (altitude_upper == UNDEFINED_ALTITUDE_CONSTRAINT && altitude_lower == UNDEFINED_ALTITUDE_CONSTRAINT) {
      m_altitude_constraint_high = MAX_ALTITUDE_CONSTRAINT;
      m_altitude_constraint_low = MIN_ALTITUDE_CONSTRAINT;
   } else if (altitude_upper == UNDEFINED_ALTITUDE_CONSTRAINT || altitude_lower == UNDEFINED_ALTITUDE_CONSTRAINT) {
      if (altitude_upper > altitude_lower) {
         m_altitude_constraint_high = altitude_upper;
      } else {
         m_altitude_constraint_high = altitude_lower;
      }
      m_altitude_constraint_low = MIN_ALTITUDE_CONSTRAINT;
   } else {
      if (altitude_upper > altitude_lower) {
         m_altitude_constraint_high = altitude_upper;
         m_altitude_constraint_low = altitude_lower;

      } else {
         m_altitude_constraint_high = altitude_lower;
         m_altitude_constraint_low = altitude_upper;
      }
   }
}

void Waypoint::ProcessSpeedConstraints(Units::Speed speed) {
   if (speed == UNDEFINED_SPEED_CONSTRAINT) {
      m_speed_constraint_high = MAX_SPEED_CONSTRAINT;
      m_speed_constraint_low = MIN_SPEED_CONSTRAINT;
   } else {
      m_speed_constraint_high = speed;
      m_speed_constraint_low = MIN_SPEED_CONSTRAINT;
   }
}

bool Waypoint::load(DecodedStream *input) {
   set_stream(input);

   const unsigned int column_count_v4_schema = 14;
   const unsigned int column_count_no_rf_legs_pre_v4_schema = 12;
   const unsigned int column_count_complete_pre_v4_schema = 15;

   bool f = load_datum(m_name);
   if (!f) {
      LoggingLoadable::report_error("could not load waypoint_name");
   }

   f = loadAngleDegrees(m_latitude);
   if (!f) {
      LoggingLoadable::report_error("could not load waypoint_Latitude");
   }

   f = loadAngleDegrees(m_longitude);
   if (!f) {
      LoggingLoadable::report_error("could not load waypoint_Longitude");
   }

   f = loadLengthFeet(m_altitude);
   if (!f) {
      LoggingLoadable::report_error("could not load waypoint_altitude");
   }

   double uninterpreted_loaded_values[column_count_complete_pre_v4_schema];
   for (int i = 4; i < column_count_v4_schema - 1; ++i) {
      bool f = load_datum(uninterpreted_loaded_values[i]);
      if (!f) {
         input->push_back();
         if (i == column_count_no_rf_legs_pre_v4_schema) {
            // use column_count_no_rf_legs_pre_v4_schema
            m_nominal_ias = Units::KnotsSpeed(uninterpreted_loaded_values[5]);
            m_mach = uninterpreted_loaded_values[6];
            m_altitude_constraint_high = Units::FeetLength(uninterpreted_loaded_values[8]);
            m_altitude_constraint_low = Units::FeetLength(uninterpreted_loaded_values[9]);
            m_speed_constraint_high = Units::KnotsSpeed(uninterpreted_loaded_values[10]);
            m_speed_constraint_low = Units::KnotsSpeed(uninterpreted_loaded_values[11]);
            m_rf_turn_arc_radius = Units::zero();
            m_rf_turn_center_latitude = Units::zero();
            m_rf_turn_center_longitude = Units::zero();
            return true;
         }
         LoggingLoadable::report_error("could not load a waypoint parameter");
         return false;
      }
   }

   // Load the next value as a string, then test its value
   std::string uninterpreted_next_value;
   f = load_datum(uninterpreted_next_value);
   if (!f) {
      LoggingLoadable::report_error("could not load a waypoint parameter...reason unknown");
      return false;
   } else {
      auto loaded_character_length = uninterpreted_next_value.size();
      const static auto leg_type_identifier_size = 2;
      if (loaded_character_length == leg_type_identifier_size) {
         // use column_count_v4_schema
         m_nominal_ias = Units::KnotsSpeed(uninterpreted_loaded_values[4]);
         m_mach = uninterpreted_loaded_values[5];
         m_altitude_constraint_high = Units::FeetLength(uninterpreted_loaded_values[6]);
         m_altitude_constraint_low = Units::FeetLength(uninterpreted_loaded_values[7]);
         m_speed_constraint_high = Units::KnotsSpeed(uninterpreted_loaded_values[8]);
         m_speed_constraint_low = Units::KnotsSpeed(uninterpreted_loaded_values[9]);
         m_rf_turn_arc_radius = Units::NauticalMilesLength(uninterpreted_loaded_values[10]);
         m_rf_turn_center_latitude = Units::DegreesAngle(uninterpreted_loaded_values[11]);
         m_rf_turn_center_longitude = Units::DegreesAngle(uninterpreted_loaded_values[12]);
         m_arinc424_leg_type = uninterpreted_next_value;
         return true;
      } else {
         // use column_count_complete_pre_v4_schema
         m_nominal_ias = Units::KnotsSpeed(uninterpreted_loaded_values[5]);
         m_mach = uninterpreted_loaded_values[6];
         m_altitude_constraint_high = Units::FeetLength(uninterpreted_loaded_values[8]);
         m_altitude_constraint_low = Units::FeetLength(uninterpreted_loaded_values[9]);
         m_speed_constraint_high = Units::KnotsSpeed(uninterpreted_loaded_values[10]);
         m_speed_constraint_low = Units::KnotsSpeed(uninterpreted_loaded_values[11]);
         m_rf_turn_arc_radius = Units::NauticalMilesLength(uninterpreted_loaded_values[12]);
         m_rf_turn_center_latitude = Units::DegreesAngle(std::strtod(uninterpreted_next_value.c_str(), NULL) );
         f = load_datum(uninterpreted_loaded_values[14]);
         if (!f) {
            LoggingLoadable::report_error("could not load a waypoint parameter");
            return false;
         }
         m_rf_turn_center_longitude = Units::DegreesAngle(uninterpreted_loaded_values[14]);
         m_arinc424_leg_type = "UNSET";
         return true;
      }
   }
}


std::ostream &operator<<(std::ostream &out,
                         const Waypoint &waypoint) {
   out << waypoint.GetName() << " ";
   out << Units::DegreesAngle(waypoint.GetLatitude()).value() << " ";
   out << Units::DegreesAngle(waypoint.GetLongitude()).value() << " ";
   out << Units::FeetLength(waypoint.GetAltitude()).value() << " ";
   out << Units::KnotsSpeed(waypoint.GetNominalIas()).value() << " ";
   out << waypoint.GetMach() << " ";
   out << Units::FeetLength(waypoint.GetAltitudeConstraintHigh()).value() << " ";
   out << Units::FeetLength(waypoint.GetAltitudeConstraintLow()).value() << " ";
   out << Units::KnotsSpeed(waypoint.GetSpeedConstraintHigh()).value() << " ";
   out << Units::KnotsSpeed(waypoint.GetSpeedConstraintLow()).value() << " ";
   out << Units::NauticalMilesLength(waypoint.GetRfTurnArcRadius()).value() << " ";
   out << Units::DegreesAngle(waypoint.GetRfTurnCenterLatitude()).value() << " ";
   out << Units::DegreesAngle(waypoint.GetRfTurnCenterLongitude()).value() << " ";
   out << waypoint.GetArinc424LegType() << " ";
   out << std::endl;
   return out;
}

std::ostream &operator<<(std::ostream &out,
                         const std::list<Waypoint> &waypoints) {
   for (std::list<Waypoint>::const_iterator i = waypoints.begin(); i != waypoints.end(); ++i) {
      out << *i;
   }
   return out;
}
