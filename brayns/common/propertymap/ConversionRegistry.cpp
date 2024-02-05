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

#include "ConversionRegistry.h"

#include "AnyConverter.h"
#include "Converter.h"
#include "EnumProperty.h"

namespace
{
brayns::AnyConverterMap _registerConverters()
{
    brayns::AnyConverterMap converters;
    converters.addexn2<int32_t, int64_t>();
    converters.addexn2<int32_t, double>();
    converters.addexn2<int64_t, double>();
    converters.addex2<int32_t, brayns::EnumProperty>();
    converters.addex2<int64_t, brayns::EnumProperty>();
    converters.addex2<std::string, brayns::EnumProperty>();
    return converters;
}

brayns::AnyConverterMap _converters = _registerConverters();
} // namespace

namespace brayns
{
bool ConversionRegistry::convert(const Any& from, Any& to)
{
    return _converters.convert(from, to);
}
} // namespace brayns