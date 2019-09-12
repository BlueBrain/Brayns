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

#include <optix_world.h>

template <typename T>
static __device__ inline T interpolateValues(const float v_min,
                                             const float v_max,
                                             const float value,
                                             optix::buffer<T, 1>& values)
{
    const int num_values = values.size();

    const float v_clamped = min(v_max, max(v_min, value));
    const float range_per_value = (v_max - v_min) / (num_values - 1);
    const float idx_value = (v_clamped - v_min) / range_per_value;

    const int index = int(floor(idx_value));

    if (index == num_values - 1)
        return values[index];

    const float v_low = v_min + float(index) * range_per_value;
    const float t = (v_clamped - v_low) / range_per_value;

    return values[index] * (1.0f - t) + values[index + 1] * t;
}

static __device__ inline float3 calcTransferFunctionColor(
    const float range_min, const float range_max, const float value,
    optix::buffer<float3, 1>& colors, optix::buffer<float, 1>& opacities)
{
    const float3 WHITE = make_float3(1.f, 1.f, 1.f);

    const float3 color_opaque =
        interpolateValues<float3>(range_min, range_max, value, colors);

    const float opacity =
        interpolateValues<float>(range_min, range_max, value, opacities);

    return opacity * color_opaque + (1.0f - opacity) * WHITE;
}
