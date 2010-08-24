/*
    Copyright 2008 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 27/06/08.

    This file is part of MRtrix.

    MRtrix is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MRtrix is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __file_dicom_study_h__
#define __file_dicom_study_h__

#include "ptr.h"
#include "file/dicom/series.h"

namespace MR {
  namespace File {
    namespace Dicom {

      class Patient;
      class Series;

      class Study : public std::vector< RefPtr<Series> > {
        public:
          Study (
              Patient* parent, 
              const String& study_name, 
              const String& study_ID = "", 
              const String& study_date = "",
              const String& study_time = "");

          Patient*     patient;
          String       name;
          String       ID;
          String       date;
          String       time;

          RefPtr<Series>   find (
              const String& series_name, 
              guint         series_number,
              const String& series_modality = "", 
              const String& series_date = "",
              const String& series_time = "");
      };


      std::ostream& operator<< (std::ostream& stream, const Study& item);







      inline Study::Study(
          Patient* parent, 
          const String& study_name, 
          const String& study_ID, 
          const String& study_date,
          const String& study_time) :
        patient (parent),
        name (study_name),
        ID (study_ID),
        date (study_date),
        time (study_time)
      {
      }



    }
  }
}


#endif



