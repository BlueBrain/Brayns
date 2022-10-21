/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(RenderImageParams)
BRAYNS_JSON_OBJECT_ENTRY(bool, send, "Send image once rendered", Default(true))
BRAYNS_JSON_OBJECT_ENTRY(bool, force, "Send image even if nothing new was rendered", Default(false))
BRAYNS_JSON_OBJECT_ENTRY(std::string, format, "Encoding of returned image data (jpg or png)", Default("jpg"))
BRAYNS_JSON_OBJECT_ENTRY(int, jpeg_quality, "Quality if using JPEG encoding", Minimum(0), Maximum(100), Default(100))
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(RenderImageResult)
BRAYNS_JSON_OBJECT_ENTRY(size_t, accumulation, "Current frame accumulation")
BRAYNS_JSON_OBJECT_ENTRY(size_t, max_accumulation, "Maximum frame accumulation")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
