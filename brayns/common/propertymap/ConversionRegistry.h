/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "Any.h"

namespace brayns
{
/**
 * @brief Static class used to store all runtime conversions between Any
 * objects without changing the stored type. For example, a conversion from
 * Any(1.0) to Any(-1) gives Any(1)
 *
 */
class ConversionRegistry
{
public:
    /**
     * @brief Convert the value contained in the source object into the
     * destination object current type and store it inside the destination.
     *
     * @param from The source object.
     * @param to The destination object.
     * @return true A converter has been found to perform the conversion.
     * @return false No converter found to perform the conversion.
     */
    static bool convert(const Any& from, Any& to);
};
} // namespace brayns