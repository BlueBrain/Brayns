/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <stdexcept>
#include <string>
#include <typeindex>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace brayns
{
/**
 * @brief The TypeNameDemangler struct demangles decorated type names (if compiled with GCC)
 */
struct TypeNameDemangler
{
    // https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
    // https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
    template<typename T>
    static std::string demangle()
    {
        std::type_index ti = typeid(std::decay_t<T>);

        #ifdef __GNUG__
        int status {};
        std::string result;

        char *demangledName = abi::__cxa_demangle(ti.name(), 0, 0, &status);

        if(status == -1)
        {
            throw std::runtime_error("Could not allocate memory to store the demangled name");
        }
        else
        {
            if(status == 0)
            {
                result = std::string(demangledName);
            }

            free(demangledName);

            if(status != 0)
            {
                throw std::runtime_error("Could not demangle type");
            }
        }

        return result;
        #else
        return std::string(ti.name());
        #endif
    }
};
}
