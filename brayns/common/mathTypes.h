/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef _mathTypes_h_
#define _mathTypes_h_

#include <vmmlib/vmmlib.hpp>

namespace brayns
{
/**
 * AABB definitions
 */
typedef vmml::AABB<float> Boxf;
typedef vmml::AABB<double> Boxd;

/**
 * Matrix definitions
 */
using vmml::Matrix4f;

using vmml::Matrix4d;
using vmml::Matrix4f;

/**
 * Vector definitions
 */
using vmml::Vector2i;
using vmml::Vector3i;

using vmml::Vector2ui;
using vmml::Vector3ui;

using vmml::Vector2f;
using vmml::Vector3f;
typedef std::vector<Vector3f> Vector3fs;
using vmml::Vector4f;
typedef std::vector<Vector4f> Vector4fs;

typedef vmml::vector<2, double> Vector2d;
typedef std::vector<Vector2d> Vector2ds;
typedef vmml::vector<3, double> Vector3d;

/**
 * Quaternion definitions
 */
using Quaterniond = vmml::Quaternion<double>; //!< Double quaternion.
}

#endif // _mathTypes_h_
