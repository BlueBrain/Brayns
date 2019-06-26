/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Author: Sebastien Speierer <sebastien.speierer@epfl.ch>
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

#include <brayns/common/types.h>

#include <brain/neuron/types.h>
#include <brain/types.h>
#include <brion/types.h>

#include <brayns/common/geometry/SDFBezier.h>

#include <vector>

namespace brayns
{
using brain::neuron::SectionType;

std::vector<SDFBezier> compute_bezier_curves(const Vector4fs& samples)
{
    int N = int(samples.size()) - 1;

    std::vector<SDFBezier> curves(N);

    std::vector<float> a(N);
    std::vector<float> b(N);
    std::vector<float> c(N);
    std::vector<Vector3f> r(N);

    // left most segment
    a[0] = 0.0f;
    b[0] = 2.0f;
    c[0] = 1.0f;
    r[0] = samples[0] + 2.0f * samples[1];

    // internal segments (build tri-diagonal matrix)
    for (int i = 1; i < N - 1; i++)
    {
        a[i] = 1.0f;
        b[i] = 4.0f;
        c[i] = 1.0f;
        r[i] = 4 * samples[i] + 2 * samples[i + 1];
    }

    // right segment
    a[N - 1] = 2.0f;
    b[N - 1] = 7.0f;
    c[N - 1] = 0.0f;
    r[N - 1] = 8 * samples[N - 1] + samples[N];

    // solves Ax=b with the Thomas algorithm (from Wikipedia)
    for (int i = 1; i < N; i++)
    {
        float m = a[i] / b[i - 1];
        b[i] = b[i] - m * c[i - 1];
        r[i] = r[i] - m * r[i - 1];
    }

    curves[N - 1].c0 = r[N - 1] / b[N - 1];

    for (int i = N - 2; i >= 0; i--)
        curves[i].c0 = (r[i] - c[i] * curves[i + 1].c0) / b[i];

    // we have c0, now compute c1
    for (int i = 0; i < N - 1; i++)
        curves[i].c1 = 2 * Vector3f(samples[i + 1]) - curves[i + 1].c0;

    curves[N - 1].c1 = 0.5 * (Vector3f(samples[N]) + curves[N - 1].c0);

    for (int i = 0; i < N; i++)
    {
        curves[i].p0 = Vector3f(samples[i]);
        curves[i].r0 = 0.5f * samples[i].w;
        curves[i].p1 = Vector3f(samples[i + 1]);
        curves[i].r1 = 0.5f * samples[i + 1].w;
    }

    // force the first point to have the same radius as the second (for soma)
    curves[0].r0 = curves[0].r1;

    return curves;
}
} // namespace brayns