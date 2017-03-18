/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
// Author: Sudeep Pillai (spillai@csail.mit.edu)
// Note: Stripped from pyxx project

#pragma once

#include <boost/python.hpp>
/*
 * Provides template support
 */

template<typename TemplateType>
struct expose_template_type
{
   // do nothing! 
};

template<typename TemplateType>
struct expose_template_type_base
{
    bool wrapped()
    {
        using namespace boost::python::converter;
        using namespace boost::python;
        registration const * p = registry::query( type_id<TemplateType>() );
        return p && (p->m_class_object || p->m_to_python);
    }

};
