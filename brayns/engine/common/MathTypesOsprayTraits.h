/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/MathTypes.h>

#include <ospray/ospray_cpp/Traits.h>

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::Vector2f, OSP_VEC2F)
OSPTYPEFOR_SPECIALIZATION(brayns::Vector2i, OSP_VEC2I)
OSPTYPEFOR_SPECIALIZATION(brayns::Vector2ui, OSP_VEC2UI)
OSPTYPEFOR_SPECIALIZATION(brayns::Vector3f, OSP_VEC3F)
OSPTYPEFOR_SPECIALIZATION(brayns::Vector3i, OSP_VEC3I)
OSPTYPEFOR_SPECIALIZATION(brayns::Vector3ui, OSP_VEC3UI)
OSPTYPEFOR_SPECIALIZATION(brayns::Vector4f, OSP_VEC4F)
}
