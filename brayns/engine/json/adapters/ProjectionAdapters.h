/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/camera/projections/Orthographic.h>
#include <brayns/engine/camera/projections/Perspective.h>

#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(Orthographic)
BRAYNS_JSON_ADAPTER_ENTRY(height, "Orthographic projection plane height")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(Perspective)
BRAYNS_JSON_ADAPTER_ENTRY(fovy, "Vertical field of view")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("aperture_radius", apertureRadius, "Lens aperture radius")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("focus_distance", focusDistance, "Camera focus distance")
BRAYNS_JSON_ADAPTER_END()
}
