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

namespace dti
{
BRAYNS_MESSAGE_BEGIN(AddStreamlinesMessage)
BRAYNS_MESSAGE_ENTRY(std::string, name, "Model name")
BRAYNS_MESSAGE_ENTRY(std::vector<uint64_t>, gids, "List of cell GIDs")
BRAYNS_MESSAGE_ENTRY(std::vector<uint64_t>, indices, "List of indices")
BRAYNS_MESSAGE_ENTRY(std::vector<float>, vertices,
                     "List of vertices (3 components per vertex)")
BRAYNS_MESSAGE_ENTRY(double, radius, "Streamline tube radius")
BRAYNS_MESSAGE_ENTRY(double, opacity, "Color opacity")
BRAYNS_MESSAGE_ENTRY(uint32_t, color_scheme,
                     "Color scheme to draw the streamlines")
BRAYNS_MESSAGE_END()
} // namespace dti
