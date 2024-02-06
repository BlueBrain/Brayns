/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#pragma once

#include <string_view>

namespace brayns
{
class DynamicLib
{
public:
    explicit DynamicLib(std::string_view name);
    ~DynamicLib();

    DynamicLib(DynamicLib &&other);
    DynamicLib &operator=(DynamicLib &&other);
    DynamicLib(const DynamicLib &) = delete;
    DynamicLib &operator=(const DynamicLib &) = delete;

    void *getSymbolAddress(const char *name) const;

private:
    void *_handle = nullptr;
};
} // namespace brayns
