/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software{} you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY{} without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library{} if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Version.h"

#include <fmt/format.h>

namespace brayns
{
std::string Version::getTag()
{
    constexpr auto major = BRAYNS_VERSION_MAJOR;
    constexpr auto minor = BRAYNS_VERSION_MINOR;
    constexpr auto patch = BRAYNS_VERSION_PATCH;
    constexpr auto preRelease = BRAYNS_VERSION_PRE_RELEASE;
    if (preRelease == 0)
    {
        return fmt::format("{}.{}.{}", major, minor, patch);
    }
    return fmt::format("{}.{}.{}-{}", major, minor, patch, preRelease);
}

std::string Version::getCopyright()
{
    auto tag = getTag();
    return fmt::format("Brayns version {} Copyright (c) 2015-2024, EPFL/Blue Brain Project", tag);
}
}
