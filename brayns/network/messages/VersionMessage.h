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

#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(VersionMessage)
BRAYNS_JSON_OBJECT_ENTRY(int, major, "Major version")
BRAYNS_JSON_OBJECT_ENTRY(int, minor, "Minor version")
BRAYNS_JSON_OBJECT_ENTRY(int, patch, "Patch level")
BRAYNS_JSON_OBJECT_ENTRY(std::string, revision, "SCM revision")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
