/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
typedef vmml::AABB<int32_t> Boxi;
typedef vmml::AABB<uint32_t> Boxui;

/**
 * Matrix definitions
 */
using vmml::Matrix3f;
using vmml::Matrix4f;

using vmml::Matrix3d;
using vmml::Matrix4d;

using vmml::Matrix4f;
typedef std::vector< Matrix4f > Matrix4fs;

/**
 * Vector definitions
 */
using vmml::Vector2i;
typedef std::vector<Vector2i> Vector2is;
using vmml::Vector3i;
typedef std::vector<Vector3i> Vector3is;
using vmml::Vector4i;
typedef std::vector<Vector4i> Vector4is;

using vmml::Vector2ui;
typedef std::vector<Vector2ui> Vector2uis;
using vmml::Vector3ui;
typedef std::vector<Vector3ui> Vector3uis;
using vmml::Vector4ui;
typedef std::vector<Vector4ui> Vector4uis;

using vmml::Vector2f;
typedef std::vector<Vector2f> Vector2fs;
using vmml::Vector3f;
typedef std::vector<Vector3f> Vector3fs;
using vmml::Vector4f;
typedef std::vector<Vector4f> Vector4fs;

typedef vmml::vector<2, double> Vector2d;
typedef std::vector<Vector2d> Vector2ds;
typedef vmml::vector<3, double> Vector3d;
typedef std::vector<Vector3d> Vector3ds;
typedef vmml::vector<4, double> Vector4d;
typedef std::vector<Vector4d> Vector4ds;

/**
 * Quaternion definitions
 */
typedef vmml::Quaternion<float> Quaternionf; //!< Float quaternion.
typedef vmml::Quaternion<double> Quaterniond; //!< Double quaternion.

}

#endif // _mathTypes_h_
