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
// Copyright 2017 The MITRE Corporation. All Rights Reserved.
// ****************************************************************************

#pragma once
#include "loader/TokenStream.h"
#include "loader/SinglePushBackStream.h"
#include "loader/IncludeStream.h"
#include "loader/CommentStream.h"
#include "loader/EchoStream.h"
#include <string>
#include "loader/FormatDifferentiatorStreamEcho.h"
#include "loader/ArchiveStreamEcho.h"
#include <Length.h>
#include <Speed.h>
#include <Time.h>
#include <Angle.h>
#include <Mass.h>
#include <Area.h>
#include <MassFlowRate.h>
#include "utility/CustomUnits.h"

class DecodedStream:  
   public SinglePushBackStream<
      FormatDifferentiatorStreamEcho<
         EchoStream<
               IncludeStream<
                  ArchiveStreamEcho<
                     CommentStream<
                        TokenStream> > > > > >
{
public:
   DecodedStream(void);
   ~DecodedStream(void);

   bool set_html_output_file(const std::string& dump_file_name)
   {
      return set_output_file(dump_file_name, 0);
   }

   /*
    * Primitive Declarations
    */
   bool get_datum(std::string &s);
   bool get_datum(short &s);
   bool get_datum(unsigned short &s);
   bool get_datum(int &s);
   bool get_datum(unsigned int &s);
   bool get_datum(long &s);
   bool get_datum(double &s);
   bool get_datum(float &s);
   bool get_datum(bool &s);

	/*
	 * Unitized declarations
	 */
	bool get_datum(Units::MetersLength &s);
	bool get_datum(Units::NauticalMilesLength &s);
	bool get_datum(Units::FeetLength &s);
	bool get_datum(Units::KilometersLength &s);
	bool get_datum(Units::MetersPerSecondSpeed &s);
	bool get_datum(Units::KnotsSpeed &s);
	bool get_datum(Units::SecondsTime &s);
	bool get_datum(Units::DegreesAngle &s);
	bool get_datum(Units::RadiansAngle &s);
	bool get_datum(Units::KilogramsMass &s);
	bool get_datum(Units::PoundsMass &s);
	bool get_datum(Units::KilogramsPerHourMassFlowRate &s);
	bool get_datum(Units::PoundsPerHourMassFlowRate &s);
	bool get_datum(Units::FeetArea &s);
	bool get_datum(Units::MetersArea &s);
	bool get_datum(Units::FeetPerMinuteSpeed &s);
	bool get_datum(Units::SecondsPerNauticalMileInvertedSpeed &s);
	bool get_datum(Units::HertzFrequency &s);
	bool get_datum(Units::MetersPerSecondSquaredLengthGain &s);


};