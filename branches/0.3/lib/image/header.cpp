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
#include "image/header.h"
#include "dataset/stride.h"
#include "math/matrix.h"
#include "math/permutation.h"
#include "image/axis.h"
#include "image/name_parser.h"
#include "image/format/list.h"
#include "image/handler/default.h"

namespace MR {
  namespace Image {

    void Header::sanitise ()
    {
      debug ("sanitising header...");

      if (axes.ndim() < 3) {
        info ("image contains fewer than 3 dimensions - adding extra dimensions");
        axes.ndim() = 3;
      }

      if (!finite (axes.vox(0)) || !finite (axes.vox(1)) || !finite (axes.vox(2))) {
        error ("invalid voxel sizes - resetting to sane defaults");
        axes.vox(0) = axes.vox(1) = axes.vox(2) = 1.0;
      }

      if (transform_matrix.is_set()) {
        if (transform_matrix.rows() != 4 || transform_matrix.columns() != 4) {
          transform_matrix.clear();
          error ("transform matrix is not 4x4 - resetting to sane defaults");
        }
        else {
          for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 4; j++) {
              if (!finite (transform_matrix(i,j))) {
                transform_matrix.clear();
                error ("transform matrix contains invalid entries - resetting to sane defaults");
                break;
              }
            }
            if (!transform_matrix.is_set()) break;
          }
        }
      }

      if (!transform_matrix.is_set()) {
        transform_matrix.allocate (4,4);
        transform_matrix.identity();
        transform_matrix(0,3) = -0.5 * (axes.dim(0)-1) * axes.vox(0);
        transform_matrix(1,3) = -0.5 * (axes.dim(1)-1) * axes.vox(1);
        transform_matrix(2,3) = -0.5 * (axes.dim(2)-1) * axes.vox(2);
      }

      transform_matrix(3,0) = transform_matrix(3,1) = transform_matrix(3,2) = 0.0; transform_matrix(3,3) = 1.0;

      Math::Permutation permutation (3);
      Math::absmax (transform_matrix.row(0).sub(0,3), permutation[0]);
      Math::absmax (transform_matrix.row(1).sub(0,3), permutation[1]);
      Math::absmax (transform_matrix.row(2).sub(0,3), permutation[2]);

      assert (permutation[0] != permutation[1] && permutation[1] != permutation[2] && permutation[2] != permutation[0]);

      bool flip[3] = {
        ( transform_matrix(0, permutation[0]) < 0.0 ),
        ( transform_matrix(1, permutation[1]) < 0.0 ),
        ( transform_matrix(2, permutation[2]) < 0.0 )
      };

      if (permutation[0] != 0 || permutation[1] != 1 || permutation[2] != 2 || flip[0] || flip[1] || flip[2]) {

        Axes::Axis a[] = { axes[permutation[0]], axes[permutation[1]], axes[permutation[2]] };
        axes[0] = a[0];
        axes[1] = a[1];
        axes[2] = a[2];

        for (size_t i = 0; i < 3; i++) {
          Math::Vector<float> row = transform_matrix.row(i).sub(0,3);
          permutation.apply (row); 
        }

        Math::Vector<float> translation = transform_matrix.column(3).sub(0,3);
        for (size_t i = 0; i < 3; i++) {
          if (flip[i]) {
            axes.stride(i) = -axes.stride(i);
            float length = float(axes[i].dim-1) * axes[i].vox;
            Math::Vector<float> axis = transform_matrix.column(i).sub(0,3);
            for (size_t n = 0; n < 3; n++) {
              axis[n] = -axis[n];
              translation[n] -= length*axis[n];
            }
          }
        }
      }

      DataSet::Stride::sanitise (axes);
      DataSet::Stride::symbolise (axes);
    }




    void Header::merge (const Header& H)
    {

      if (dtype != H.dtype) 
        throw Exception ("data types differ between image files for \"" + name() + "\"");

      if (offset != H.offset || scale != H.scale) 
        throw Exception ("scaling coefficients differ between image files for \"" + name() + "\"");

      if (ndim() != H.ndim()) 
        throw Exception ("dimension mismatch between image files for \"" + name() + "\"");

      for (size_t n = 0; n < ndim(); n++) {
        if (dim(n) != H.dim(n)) 
          throw Exception ("dimension mismatch between image files for \"" + name() + "\"");

        if (axes.stride(n) != H.axes.stride(n))
          throw Exception ("data layout differs image files for \"" + name() + "\"");

        if (axes.vox(n) != H.axes.vox(n))
          error ("WARNING: voxel dimensions differ between image files for \"" + name() + "\"");
      }

      for (std::vector<File::Entry>::const_iterator item = H.files.begin(); item != H.files.end(); item++)
        files.push_back (*item);

      for (std::map<std::string, std::string>::const_iterator item = H.begin(); item != H.end(); item++)
        if (std::find (begin(), end(), *item) == end())
          insert (*item);

      for (std::vector<std::string>::const_iterator item = H.comments.begin(); item != H.comments.end(); item++)
        if (std::find (comments.begin(), comments.end(), *item) == comments.end())
          comments.push_back (*item);

      if (!transform_matrix.is_set() && H.transform_matrix.is_set()) transform_matrix = H.transform_matrix;
      if (!DW_scheme.is_set() && H.DW_scheme.is_set()) DW_scheme = H.DW_scheme; 
    }







    const Header Header::open (const std::string& image_name, bool read_write)
    {
      if (image_name.empty()) throw Exception ("no name supplied to open image!");
      Header H;
      H.readwrite = read_write;

      try {
        info ("opening image \"" + image_name + "\"...");

        ParsedNameList list;
        std::vector<int> num = list.parse_scan_check (image_name);

        const Format::Base** format_handler = Format::handlers;
        std::vector< RefPtr<ParsedName> >::iterator item = list.begin();
        H.identifier = (*item)->name();

        for (; *format_handler; format_handler++) if ((*format_handler)->read (H)) break;
        if (!*format_handler) throw Exception ("unknown format for image \"" + H.name() + "\"");
        H.format = (*format_handler)->description;

        Header header (H);
        while (++item != list.end()) {
          header.name() = (*item)->name();
          if (!(*format_handler)->read (header)) throw Exception ("image specifier contains mixed format files");
          H.merge (header);
        }

        if (num.size()) {
          int a = 0, n = 0;
          for (size_t i = 0; i < H.axes.ndim(); i++) if (H.axes.stride(i)) n++;
          H.axes.ndim() = n + num.size();

          for (std::vector<int>::const_iterator item = num.begin(); item != num.end(); item++) {
            while (H.axes[a].stride) a++;
            H.axes.dim(a) = *item;
            H.axes.stride(a) = ++n;
          }
        }

        H.sanitise();
        if (!H.handler) H.handler = new Handler::Default (H, false);

        H.identifier = image_name;
      }
      catch (Exception& E) { throw Exception (E, "error opening image \"" + image_name + "\""); }

      return (H);
    }





    const Header Header::create (const std::string& image_name, const Header& template_header)
    {
      if (image_name.empty()) throw Exception ("no name supplied to open image!");
      Header H (template_header);
      H.readwrite = true;
      DataSet::Stride::symbolise (H.axes);

      try {
        info ("creating image \"" + image_name + "\"...");

        H.sanitise();

        NameParser parser;
        parser.parse (image_name);
        std::vector<int> Pdim (parser.ndim());

        int Hdim [H.ndim()];
        for (size_t i = 0; i < H.ndim(); i++) Hdim[i] = H.dim(i);

        H.identifier = image_name;

        const Format::Base** format_handler = Format::handlers;
        for (; *format_handler; format_handler++) {
          if ((*format_handler)->check (H, H.ndim() - Pdim.size())) break;
        }
        if (!*format_handler) throw Exception ("unknown format for image \"" + image_name + "\"");
        H.format = (*format_handler)->description;

        H.dtype.set_byte_order_native();
        int a = 0;
        for (size_t n = 0; n < Pdim.size(); n++) {
          while (H.axes.stride(a)) a++;
          Pdim[n] = Hdim[a];
        }
        parser.calculate_padding (Pdim);

        Header header (H);
        std::vector<int> num (Pdim.size());

        if (image_name != "-") H.name() = parser.name (num);
        (*format_handler)->create (H);

        while (get_next (num, Pdim)) {
          header.name() = parser.name (num);
          (*format_handler)->create (header);
          H.merge (header);
        }

        if (Pdim.size()) {
          int a = 0, n = 0;
          for (size_t i = 0; i < H.ndim(); i++) if (H.axes.stride(i)) n++;
          H.axes.ndim() = n + Pdim.size();

          for (std::vector<int>::const_iterator item = Pdim.begin(); item != Pdim.end(); item++) {
            while (H.axes.stride(a)) a++;
            H.axes.dim(a) = *item;
            H.axes.stride(a) = ++n;
          }
        }

        H.sanitise();
        if (!H.handler) H.handler = new Handler::Default (H, false);

        H.identifier = image_name;
      }
      catch (Exception& E) { throw Exception (E, "error creating image \"" + image_name + "\""); }

      return (H);
    }













    std::string Header::description() const
    {
      std::string desc ( 
            "************************************************\n"
            "Image:               \"" + name() + "\"\n"
            "************************************************\n"
            "  Format:            " + ( format ? format : "undefined" ) + "\n"
            "  Dimensions:        ");

      size_t i;
      for (i = 0; i < axes.ndim(); i++) {
        if (i) desc += " x ";
        desc += str (axes.dim(i));
      }



      desc += "\n  Voxel size:        ";

      for (i = 0; i < axes.ndim(); i++) {
        if (i) desc += " x ";
        desc += isnan (axes.vox(i)) ? "?" : str (axes.vox(i));
      }




      desc += "\n  Dimension labels:  ";

      for (i = 0; i < axes.ndim(); i++)  
        desc += ( i ? "                     " : "" ) + str (i) + ". " 
          + ( axes.description(i).size() ? axes.description(i) : "undefined" ) + " ("
          + ( axes.units(i).size() ? axes.units(i) : "?" ) + ")\n";



      desc += std::string ("  Data type:         ") + ( dtype.description() ? dtype.description() : "invalid" ) + "\n"
            "  Data layout:       [ ";


      for (i = 0; i < axes.ndim(); i++) 
        desc += axes.stride(i) ? ( axes.forward(i) ? '+' : '-' ) + str (abs(axes.stride(i))-1) + " " : "? ";



      desc += "]\n"
            "  Data scaling:      offset = " + str (offset) + ", multiplier = " + str (scale) + "\n"
            "  Comments:          " + ( comments.size() ? comments[0] : "(none)" ) + "\n";



      for (i = 1; i < comments.size(); i++)
        desc += "                     " + comments[i] + "\n";

      if (size() > 0) {
        desc += "  Properties:\n";
        for (std::map<std::string, std::string>::const_iterator it = begin(); it != end(); ++it)
          desc += "    " + it->first + ": " + it->second + "\n";
      }

      if (transform_matrix.is_set()) {
        desc += "  Transform:         ";
        for (size_t i = 0; i < transform_matrix.rows(); i++) {
          if (i) desc +=  "                     ";
          for (size_t j = 0; j < transform_matrix.columns(); j++) {
            char buf[14], buf2[14];
            snprintf (buf, 14, "%.4g", transform_matrix(i,j));
            snprintf (buf2, 14, "%12.10s", buf);
            desc += buf2;
          }
          desc += "\n";

        }
      }

      if (DW_scheme.is_set()) 
        desc += "  DW scheme:         " + str (DW_scheme.rows()) + " x " + str (DW_scheme.columns()) + "\n";

      return (desc);
    }



    std::ostream& operator<< (std::ostream& stream, const Header& H)
    {
      stream << H.description();
      return (stream);
    }

  }
}
