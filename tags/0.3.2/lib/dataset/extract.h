/*
    Copyright 2009 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 02/11/09.

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

#ifndef __dataset_extract_h__
#define __dataset_extract_h__

#include "math/matrix.h"

namespace MR {
  namespace DataSet {

    //! \addtogroup DataSet
    // @{

    template <class Set> class Extract {
      public:
        typedef typename Set::value_type value_type;

        Extract (Set& original, const std::vector<std::vector<int> >& positions) : D (original), x (new size_t [ndim()]), P (positions) { }
        ~Extract () { delete [] x; }
        const std::string& name () const { return (D.name()); }
        size_t  ndim () const { return (D.ndim()); }
        int     dim (size_t axis) const { return (P[axis].size()); }
        float   vox (size_t axis) const { return (D.vox (axis)); }
        const Math::Matrix<float>& transform () const { return (D.transform()); }
        const size_t* layout () const { return (D.layout()); }

        void reset () { memset (x, 0, sizeof(size_t)*ndim()); for (size_t a = 0; a < ndim(); ++a) D.pos (a, P[a][0]); }

        ssize_t pos (size_t axis) const { return (x[axis]); } 
        void    pos (size_t axis, ssize_t position) const { x[axis] = position; D.pos (axis, P[axis][position]); }
        void    move (size_t axis, ssize_t increment) const { x[axis] += increment; D.pos (axis, P[axis][x[axis]]); }

        value_type   value () const { return (D.value()); }
        void         value (value_type val) { D.value (val); }

      private:
        Set& D;
        size_t* x;
        const std::vector<std::vector<int> > P;
    };
    
    //! @}
  }
}

#endif

