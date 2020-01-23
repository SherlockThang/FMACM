// ****************************************************************************
// NOTICE
//
// This is the copyright work of The MITRE Corporation, and was produced
// for the U. S. Government under Contract Number DTFAWA-10-C-00080, and
// is subject to Federal Aviation Administration Acquisition Management
// System Clause 3.5-13, Rights In Data-General, Alt. III and Alt. IV
// (Oct. 1996).  No other use other than that granted to the U. S.
// Government, or to those acting on behalf of the U. S. Government,
// under that Clause is authorized without the express written
// permission of The MITRE Corporation. For further information, please
// contact The MITRE Corporation, Contracts Office, 7515 Colshire Drive,
// McLean, VA  22102-7539, (703) 983-6000. 
//
// Copyright 2020 The MITRE Corporation. All Rights Reserved.
// ****************************************************************************

#include "framework/TestFrameworkScenario.h"
#include <stdio.h>
#include <cstring>
#include "math/CustomMath.h"
#include "aaesim/Bada.h"
#include "loader/Loadable.h"
#include "utility/constants.h"
#include "public/StandardAtmosphere.h"
#include "public/WindZero.h"
#include <list>

using std::string;

const Units::SecondsTime TestFrameworkScenario::mDefaultSimulationTimeStep = Units::SecondsTime(1.0);
const int TestFrameworkScenario::number_of_iterations = 1;
const int TestFrameworkScenario::number_of_aircraft = 1;

TestFrameworkScenario::TestFrameworkScenario() {

   m_mean_inter_delivery_time = 0;
   m_stdev_inter_delivery_time = 0;

   m_weather = WeatherTruth(std::shared_ptr<Wind>(),
                            std::shared_ptr<Atmosphere>(new StandardAtmosphere(Units::CelsiusTemperature(0))));
   m_weather.east_west.SetBounds(1, 5);
   m_weather.north_south.SetBounds(1, 5);
}

TestFrameworkScenario::~TestFrameworkScenario() = default;

bool TestFrameworkScenario::load(DecodedStream *input) {
   string bada_data_path;
   string wind_truth_file;
   string wind_forecast_file;
   int predictedWindOpt = 0;
   bool blendWind = true;

   set_stream(input);
   register_var("bada_data_path", &bada_data_path, true); // required
   register_named_vector_item("aircraft", &m_master_aircraft_list, true); // required

   complete();

   PostLoad(bada_data_path, predictedWindOpt, blendWind,
            mDefaultSimulationTimeStep);

   return true;
}

void TestFrameworkScenario::PostLoad(const string &bada_data_path,
                                     int predicted_wind_opt,
                                     bool blend_wind,
                                     Units::Time simulation_time_step) {

   strcpy(Bada::input_path, bada_data_path.c_str());

   InternalObserver *internalObserver = InternalObserver::getInstance();
   internalObserver->set_scenario_name(GetScenarioName());

   PostLoadAircraft(simulation_time_step, predicted_wind_opt, blend_wind);

   SimulationTime::set_simulation_time_step(simulation_time_step);
}

void TestFrameworkScenario::PostLoadAircraft(Units::Time simulation_time_step,
                                             int predicted_wind_opt,
                                             bool blend_wind) {
   for (auto &aircraft : m_master_aircraft_list) {
      aircraft.PostLoad(simulation_time_step, predicted_wind_opt, blend_wind, m_weather);
   }
}


void TestFrameworkScenario::LoadOneScenarioFromScenarioClassIntoActorLists() {
   m_aircraft_list = m_master_aircraft_list;

   int start_time = 0;

   for (auto& aircraft : m_aircraft_list) {
      aircraft.m_start_time = start_time;
      start_time += static_cast<int>(Scenario::m_rand.TruncatedGaussianSample(m_mean_inter_delivery_time,
                                                                              m_stdev_inter_delivery_time, 3.));

      aircraft.Initialize(Units::NauticalMilesLength(99), m_weather);
   }
}

void TestFrameworkScenario::ProcessOneScenario() {
   for (int i = 0; i < number_of_iterations; i++) {
      LoadOneScenarioFromScenarioClassIntoActorLists();

      InitializeIterationState(number_of_aircraft);

      ProcessOneIteration();
   }
}

void TestFrameworkScenario::InitializeIterationState(int number_of_aircraft_in) {
   InitializeIterationMetrics(number_of_aircraft_in);
}

void TestFrameworkScenario::InitializeIterationMetrics(int number_of_aircraft_in) {
   InternalObserver::getInstance()->initializeIteration(number_of_aircraft_in);
}

void TestFrameworkScenario::ProcessOneIteration() {
   std::string scenName = GetScenarioName();
   int findResult = scenName.find("-scenario");
   if (findResult >= 0) {
      scenName = scenName.replace(findResult, findResult + 9, "");
   }
   m_acstates = fopen(scenName.append("_AcStates.csv").c_str(), "w");

   fprintf(m_acstates, "Time[sec], DTG[m], V(tas)[m/s], vRate[m/s], x[m], y[m], h[m], gs[fps]\n");

   SimulationTime time;
   time.init();
   bool iteration_finished = false;

   while (!iteration_finished) {
      iteration_finished = ProcessOneCycle(time);

      time.increment();
   }
}

bool TestFrameworkScenario::ProcessOneCycle(SimulationTime &time) {
   return ProcessAllAircraft(time);
}

bool TestFrameworkScenario::ProcessAllAircraft(SimulationTime &time) {
   bool iteration_finished = true;

   for (auto& aircraft : m_aircraft_list) {
      bool aircraft_iteration_finished = aircraft.Update(time);

      if (!aircraft_iteration_finished) {
         AircraftState state = aircraft.m_truth_state_vector_old;
         RecordState(state);
      }

      iteration_finished = iteration_finished && aircraft_iteration_finished;
   }
   return iteration_finished;
}

void TestFrameworkScenario::RecordState(const AircraftState &aircraft_state) const {
   double v_mps = hypot(aircraft_state.m_xd * FEET_TO_METERS - aircraft_state.m_Vwx,
                        aircraft_state.m_yd * FEET_TO_METERS - aircraft_state.m_Vwy)
                  / cos(aircraft_state.m_gamma);

   fprintf(m_acstates, "%.0f,%.5f,%.3f,%.3f,%.5f,%.5f,%.5f,%.5f\n",
           aircraft_state.m_time,
           aircraft_state.m_distance_to_go_meters,
           v_mps,
           aircraft_state.m_zd * FEET_TO_METERS,
           aircraft_state.m_x * FEET_TO_METERS,
           aircraft_state.m_y * FEET_TO_METERS,
           aircraft_state.m_z * FEET_TO_METERS,
           Units::FeetPerSecondSpeed(aircraft_state.GetGroundSpeed()).value());
   fflush(m_acstates);
}
