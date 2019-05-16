/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <optixu/optixu_vector_types.h>

#define BASIC_LIGHT_TYPE_POINT 0
#define BASIC_LIGHT_TYPE_DIRECTIONAL 1

struct BasicLight
{
    ::optix::float3 pos;
    ::optix::float3 color;
    int casts_shadow;
    int type;
};

struct PerRayData_radiance
{
    ::optix::float3 result;
    float importance;
    int depth;
    ::optix::float3 rayDdx;
    ::optix::float3 rayDdy;
};
