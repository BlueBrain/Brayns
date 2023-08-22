/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <rkcommon/math/AffineSpace.h>
#include <rkcommon/math/Quaternion.h>
#include <rkcommon/math/vec.h>

namespace brayns
{
namespace math = rkcommon::math;

/**
 * Matrix definitions
 */
using RotationMatrix = math::LinearSpace3f;
using TransformMatrix = math::AffineSpace3f;

/**
 * Vector definitions
 */
using Vector2i = math::vec2i;
using Vector2l = math::vec2l;
using Vector3i = math::vec3i;
using Vector3l = math::vec3l;

using Vector2ui = math::vec2ui;
using Vector2ul = math::vec2ul;
using Vector3ui = math::vec3ui;
using Vector3ul = math::vec3ul;

using Vector2f = math::vec2f;
using Vector3f = math::vec3f;
using Vector4f = math::vec4f;

/**
 * Quaternion definitions
 */
using Quaternion = math::quaternionf;

} // namespace brayns
