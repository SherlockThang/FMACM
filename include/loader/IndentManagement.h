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

#pragma once

#include <string>

class IndentManagement
{
public:

   IndentManagement(void) {
      i = 0;
   }//--------------------------------------------------------

   ~IndentManagement(void) {
   }//--------------------------------------------------------

   inline void tab_In() {
      i++;
      s = s + " ";
   }//--------------------------------------------------------

   inline void tab_Out() {
      i--;
      int tabs = get_Tab();
      if (tabs == 0) {
         s = "";
      } else {
         s = "";
         while (tabs > 0) {
            s = s + " ";
            tabs--;
         }
      }
   }//--------------------------------------------------------

   inline int get_Tab() {
      return i;
   }//--------------------------------------------------------

   inline void set_Tab(int tab) {
      i = tab;
   }//--------------------------------------------------------

   inline std::string get_Space() {
      return s;
   }

protected://--------------------------------------------------------

   int i;
   std::string s;

};
