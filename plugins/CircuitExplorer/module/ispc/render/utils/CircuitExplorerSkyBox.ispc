/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include <ospray/SDK/math/vec.ih>
#include <ospray/SDK/render/Renderer.ih>

#include "../../../../../../engines/ospray/ispc/render/utils/Consts.ih"

#include "../CircuitExplorerMaterial.ih"

vec4f skyboxMapping(const uniform Renderer* uniform renderer, const Ray& ray,
                    const uniform CircuitExplorerMaterial* uniform bgMaterial)
{
    if (!bgMaterial)
        return make_vec4f(0.f);

    vec4f result = make_vec4f(bgMaterial->Kd);

    const float radius = 1e6f;
    if (!valid(bgMaterial->map_Kd))
        return result;

    // solve the equation sphere-ray to find the intersections
    const float a = 2.f * dot(ray.dir, ray.dir);
    const float b = 2.f * dot(ray.org, ray.dir);
    const float c = dot(ray.org, ray.org) - radius * radius;
    const float d = b * b - 2.f * a * c;

    if (d <= 0.f || a == 0.f)
        return result;

    const float r = sqrt(d);
    const float t1 = (-b - r) / a;
    const float t2 = (-b + r) / a;

    const float epsilon = 0.00001f;
    if (t1 <= epsilon && t2 <= epsilon)
        return result; // both intersections are behind the ray origin

    float t = 0.f;
    if (t1 <= epsilon)
        t = t2;
    else if (t2 <= epsilon)
        t = t1;
    else
        t = (t1 < t2) ? t1 : t2;

    if (t < epsilon || t > ray.t)
        return result; // Too close to intersection

    const vec3f intersection = normalize(ray.org + t * ray.dir);

    // Intersection found, now get skybox color
    varying DifferentialGeometry dg;
    dg.st =
        make_vec2f(((atan2(intersection.x, intersection.z) / PI) + 1.f) * .5f,
                   -(asin(intersection.y) / PI) + .5f);

    return get4f(bgMaterial->map_Kd, dg);
}
