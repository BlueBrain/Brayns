/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/json/MessageAdapter.h>
#include <brayns/utils/StringUtils.h>

#include "DTITypes.h"

namespace brayns
{
BRAYNS_ADAPTER_ENUM(ColorScheme, {{"None", ColorScheme::none},
                                  {"By id", ColorScheme::by_id},
                                  {"By normal", ColorScheme::by_normal}})
}

BRAYNS_MESSAGE_BEGIN(DTILoaderParameters)
BRAYNS_MESSAGE_ENTRY(float, radius, "Connectivity streamlines radius")
BRAYNS_MESSAGE_ENTRY(float, opacity, "Geometry opacity")
BRAYNS_MESSAGE_ENTRY(ColorScheme, color_scheme, "Scheme to color the geometry")
BRAYNS_MESSAGE_END()
