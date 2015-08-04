/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include <ospray/camera/Camera.h>

using namespace ospray;

namespace ispc
{
struct StereoCamera_Data;
} // ::ispc

namespace brayns
{

struct StereoCamera : public Camera
{
    StereoCamera();
    virtual std::string toString() const { return "ospray::StereoCamera"; }
    virtual void commit();

    vec3f  pos;
    vec3f  dir;
    vec3f  up;
    float  near;
    float  far;
    float  fovy;
    float  aspect;
    float  nearClip;
    float  stereo;
    float  separation;
    vec3f dir_00;
    vec3f dir_du;
    vec3f dir_dv;

    ::ispc::StereoCamera_Data *ispcData;

    virtual void initRay(Ray &ray, const vec2f &sample);
};

} // ::brayns
