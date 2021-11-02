/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <brayns/json/Message.h>

BRAYNS_MESSAGE_BEGIN(AddGridMessage)
BRAYNS_MESSAGE_ENTRY(double, min_value,
                     "Negative square grid length from world origin")
BRAYNS_MESSAGE_ENTRY(double, max_value,
                     "Positive square grid length from world origin")
BRAYNS_MESSAGE_ENTRY(double, steps, "Number of divisions")
BRAYNS_MESSAGE_ENTRY(double, radius,
                     "Radius of the cylinder that will be placed at each cell")
BRAYNS_MESSAGE_ENTRY(double, plane_opacity, "Opacity of the grid mesh material")
BRAYNS_MESSAGE_ENTRY(bool, show_axis, "Wether to show a world aligned axis")
BRAYNS_MESSAGE_ENTRY(bool, use_colors, "Use colors on the grid axes")
BRAYNS_MESSAGE_END()