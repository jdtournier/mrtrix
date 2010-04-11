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

#include "app.h"
#include "progressbar.h"
#include "image/voxel.h"
#include "image/axis.h"
#include "dataset/copy.h"
#include "dataset/extract.h"
#include "dataset/reorder.h"

using namespace MR; 

SET_VERSION_DEFAULT;

DESCRIPTION = {
  "perform conversion between different file types and optionally extract a subset of the input image.",
  "If used correctly, this program can be a very useful workhorse. In addition to converting images between different formats, it can be used to extract specific studies from a data set, extract a specific region of interest, flip the images, or to scale the intensity of the images.",
  NULL
};

ARGUMENTS = {
  Argument ("input", "input image", "the input image.").type_image_in (),
  Argument ("ouput", "output image", "the output image.").type_image_out (),
  Argument::End
};


const char* type_choices[] = { "REAL", "IMAG", "MAG", "PHASE", "COMPLEX", NULL };

OPTIONS = {
  Option ("coord", "select coordinates", "extract data only at the coordinates specified.", Optional | AllowMultiple)
    .append (Argument ("axis", "axis", "the axis of interest").type_integer (0, INT_MAX, 0))
    .append (Argument ("coord", "coordinates", "the coordinates of interest").type_sequence_int()),

  Option ("vox", "voxel size", "change the voxel dimensions.")
    .append (Argument ("sizes", "new dimensions", "A comma-separated list of values. Only those values specified will be changed. For example: 1,,3.5 will change the voxel size along the x & z axes, and leave the y-axis voxel size unchanged.")
        .type_sequence_float ()),

  Option ("datatype", "data type", "specify output image data type.")
    .append (Argument ("spec", "specifier", "the data type specifier.").type_choice (DataType::identifiers)),

  Option ("scale", "scaling factor", "apply scaling to the intensity values.")
    .append (Argument ("factor", "factor", "the factor by which to multiply the intensities.").type_float (NAN, NAN, 1.0)),

  Option ("offset", "offset", "apply offset to the intensity values.")
    .append (Argument ("bias", "bias", "the value of the offset.").type_float (NAN, NAN, 0.0)),

  Option ("zero", "replace NaN by zero", "replace all NaN values with zero."),

  Option ("output", "output type", "specify type of output")
    .append (Argument ("type", "type", "type of output.")
        .type_choice (type_choices)),

  Option ("layout", "data layout", "specify the layout of the data in memory. The actual layout produced will depend on whether the output image format can support it.")
    .append (Argument ("spec", "specifier", "the data layout specifier.").type_string ()),

  Option ("prs", "DW gradient specified as PRS", "assume that the DW gradients are specified in the PRS frame (Siemens DICOM only)."),

  Option::End
};




template <class Set, class Set2> void copy_replace_NaN_kernel (Set& destination, Set2& source) { 
  typedef typename Set::value_type T;
  T val = source.value();
  destination.value (isnan(val) ? 0.0 : val);
}


template <class Set, class Set2> void copy (Set& destination, Set2& source, bool replace_NaN) 
{ 
  std::string progress_message ("copying from \"" + source.name() + "\" to \"" + destination.name() + "\"...");

  typedef DataSet::Reorder<Set> S1;
  typedef DataSet::Reorder<Set2> S2;

  S1 dest (destination, NULL, destination.name());
  S2 src (source, dest.layout(), source.name());

  if (replace_NaN) DataSet::loop2 (progress_message, DataSet::copy_kernel<S1,S2>, dest, src);
  else DataSet::loop2 (progress_message, copy_replace_NaN_kernel<S1,S2>, dest, src);
}



EXECUTE {
  std::vector<OptBase> opt = get_options (1); // vox
  std::vector<float> vox;
  if (opt.size()) 
    vox = parse_floats (opt[0][0].get_string());

  opt = get_options (3); // scale
  float scale = 1.0;
  if (opt.size()) scale = opt[0][0].get_float();

  opt = get_options (4); // offset
  float offset = 0.0;
  if (opt.size()) offset = opt[0][0].get_float();

  opt = get_options (5); // zero
  bool replace_NaN = opt.size();

  opt = get_options (6); // output
  Image::OutputType output_type = Image::Default;
  if (opt.size()) {
    switch (opt[0][0].get_int()) {
      case 0: output_type = Image::Real; break;
      case 1: output_type = Image::Imaginary; break;
      case 2: output_type = Image::Magnitude; break;
      case 3: output_type = Image::Phase; break;
      case 4: output_type = Image::RealImag; break;
    }
  }




  const Image::Header header_in = argument[0].get_image ();
  Image::Header header (header_in);

  if (output_type == 0) {
    if (header_in.is_complex()) output_type = Image::RealImag;
    else output_type = Image::Default;
  }

  if (output_type == Image::RealImag) header.datatype() = DataType::CFloat32;
  else if (output_type == Image::Phase) header.datatype() = DataType::Float32;
  else header.datatype().unset_flag (DataType::Complex);

  
  opt = get_options (2); // datatype
  if (opt.size()) header.datatype().parse (DataType::identifiers[opt[0][0].get_int()]);

  for (size_t n = 0; n < vox.size(); n++) 
    if (std::isfinite (vox[n])) header.axes.vox(n) = vox[n];

  opt = get_options (7); // layout
  if (opt.size()) {
    std::vector<Image::Axes::Order> ax = parse_axes_specifier (header.axes, opt[0][0].get_string());
    if (ax.size() != header.axes.ndim()) 
      throw Exception (std::string("specified layout \"") + opt[0][0].get_string() + "\" does not match image dimensions");

    for (size_t i = 0; i < ax.size(); i++) {
      header.axes.order(i) = ax[i].order;
      header.axes.forward(i) = ax[i].forward;
    }
  }


  opt = get_options (8); // prs
  if (opt.size() && header.DW_scheme.rows() && header.DW_scheme.columns()) {
    for (size_t row = 0; row < header.DW_scheme.rows(); row++) {
      double tmp = header.DW_scheme(row, 0);
      header.DW_scheme(row, 0) = header.DW_scheme(row, 1);
      header.DW_scheme(row, 1) = tmp;
      header.DW_scheme(row, 2) = -header.DW_scheme(row, 2);
    }
  }

  std::vector<std::vector<int> > pos;

  opt = get_options (0); // coord
  for (size_t n = 0; n < opt.size(); n++) {
    pos.resize (header.ndim());
    int axis = opt[n][0].get_int();
    if (pos[axis].size()) throw Exception ("\"coord\" option specified twice for axis " + str (axis));
    pos[axis] = parse_ints (opt[n][1].get_string());
  }


  header.apply_scaling (scale, offset);

  assert (!header_in.is_complex());
  Image::Voxel<float> in (header_in);

  if (pos.size()) { 
    // extract specific coordinates:
    for (size_t n = 0; n < header_in.ndim(); n++) {
      if (pos[n].empty()) { 
        pos[n].resize (header_in.dim(n));
        for (size_t i = 0; i < pos[n].size(); i++) pos[n][i] = i;
      }
    }
    DataSet::Extract<Image::Voxel<float> > extract (in, pos);
    for (size_t n = 0; n < extract.ndim(); ++n)
      header.axes.dim(n) = extract.dim(n);
    const Image::Header header_out = argument[1].get_image (header);
    Image::Voxel<float> out (header_out);
    copy (out, extract, replace_NaN);
  }
  else { 
    // straight copy:
    const Image::Header header_out = argument[1].get_image (header);
    Image::Voxel<float> out (header_out);
    copy (out, in, replace_NaN);
  }
}




