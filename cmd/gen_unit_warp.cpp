/*
    Copyright 2009 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 14/08/09.

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

#include <fstream>
#include <glibmm/stringutils.h>

#include "app.h"
#include "get_set.h"
#include "dwi/tractography/file.h"
#include "dwi/tractography/properties.h"

using namespace MR; 
using namespace MR::DWI; 
using namespace std; 

SET_VERSION_DEFAULT;

DESCRIPTION = {
  "generate a warp field corresponding to a no-warp operation. This is useful to obtain the warp fields from other normalisation applications, by applying the warp of interest to the the warp field generated by this program.",
  NULL
};

ARGUMENTS = {
  Argument ("template", "template image", "the image whose warp field is to be generated.").type_image_in(),
  Argument ("warp", "output warp image", "the output warp image").type_image_out(),
  Argument::End
};



OPTIONS = { Option::End };




EXECUTE {
  Image::Interp interp (*argument[0].get_image());
  Image::Header header (interp.image);

  header.axes.set_ndim (4);
  header.axes.dim[3] = 3;
  header.offset = 0.0;
  header.scale = 1.0;
  header.data_type = DataType::Float32;

  Image::Position warp (*argument[1].get_image (header));

  ProgressBar::init (warp.voxel_count(), "generating unit warp...");

  for (warp.set(2,0); warp[2] < warp.dim(2); warp.inc(2)) {
    for (warp.set(1,0); warp[1] < warp.dim(1); warp.inc(1)) {
      for (warp.set(0,0); warp[0] < warp.dim(0); warp.inc(0)) {
        Point pos = interp.P2R (Point (warp[0], warp[1], warp[2]));
        warp.set(3,0); warp.value (pos[0]);
        warp.inc(3); warp.value (pos[1]);
        warp.inc(3); warp.value (pos[2]);
        ProgressBar::inc();
      }
    }
  }
  ProgressBar::done();
}

