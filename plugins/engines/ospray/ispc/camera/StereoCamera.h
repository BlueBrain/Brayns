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

#pragma once

#include <ospray/SDK/camera/Camera.h>

namespace ispc
{
struct StereoCamera_Data;
} // ::ispc

namespace brayns
{

struct StereoCamera : public ospray::Camera
{
    StereoCamera();
    std::string toString() const final { return "ospray::StereoCamera"; }
    void commit() final;

    ospray::vec3f  pos;
    ospray::vec3f  dir;
    ospray::vec3f  up;
    float  near;
    float  far;
    float  fovy;
    float  aspect;
    float  nearClip;
    float  stereo;
    float  separation;
    ospray::vec3f dir_00;
    ospray::vec3f dir_du;
    ospray::vec3f dir_dv;

    ::ispc::StereoCamera_Data *ispcData;
};

} // ::brayns
