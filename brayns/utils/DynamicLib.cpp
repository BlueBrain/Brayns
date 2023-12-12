/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

#include <dlfcn.h>
#include <sys/times.h>

#include "DynamicLib.h"

namespace brayns
{
DynamicLib::DynamicLib(std::string_view name)
{
    auto filename = "lib" + std::string(name) + ".so";
    _handle = dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!_handle)
    {
        std::string errorMessage = dlerror();
        throw std::runtime_error("Error opening dynamic library: " + filename + ": " + errorMessage);
    }
}

DynamicLib::~DynamicLib()
{
    if (!_handle || _handle == RTLD_DEFAULT)
    {
        return;
    }
    dlclose(_handle);
}

DynamicLib::DynamicLib(DynamicLib &&other)
{
    _handle = std::exchange(other._handle, nullptr);
}

DynamicLib &DynamicLib::operator=(DynamicLib &&other)
{
    std::swap(_handle, other._handle);
    return *this;
}

void *DynamicLib::getSymbolAddress(const char *name) const
{
    if (!_handle)
    {
        return nullptr;
    }
    return dlsym(_handle, name);
}
} // namespace brayns
