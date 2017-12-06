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

#include <ospray/SDK/render/Renderer.ih>
#include <ospray/SDK/render/util.ih>

#include <plugins/engines/ospray/ispc/render/ExtendedOBJMaterial.ih>
#include <plugins/engines/ospray/ispc/render/utils/Consts.ih>

vec4f skyboxMapping(const uniform Renderer* uniform renderer, Ray& ray,
                    const uniform unsigned int32 nbMaterials,
                    const uniform ExtendedOBJMaterial* uniform* uniform
                        materials)
{
    varying vec4f result = make_vec4f(renderer->bgColor);
    if (nbMaterials <= MATERIAL_SKYBOX)
        return result;

    const varying float radius = 1e6;
    const uniform ExtendedOBJMaterial* uniform mat =
        (const uniform ExtendedOBJMaterial* uniform)materials[MATERIAL_SKYBOX];
    if (!valid(mat->map_Kd))
        return result;

    // solve the equation sphere-ray to find the intersections
    const varying float a = 2.f * dot(ray.dir, ray.dir);
    const varying float b = 2.f * dot(ray.org, ray.dir);
    const varying float c = dot(ray.org, ray.org) - radius * radius;
    const varying float d = b * b - 2.f * a * c;

    if (d <= 0.f || a == 0.f)
        return result;

    const varying float r = sqrt(d);
    const varying float t1 = (-b - r) / a;
    const varying float t2 = (-b + r) / a;

    if (t1 <= renderer->epsilon && t2 <= renderer->epsilon)
        return result; // both intersections are behind the ray origin

    varying float t = 0.f;
    if (t1 <= renderer->epsilon)
        t = t2;
    else if (t2 <= renderer->epsilon)
        t = t1;
    else
        t = (t1 < t2) ? t1 : t2;

    if (t < renderer->epsilon)
        return result; // Too close to intersection

    const varying vec3f intersection = normalize(ray.org + t * ray.dir);

    // Intersection found, now get skybox color
    const varying vec2f
        st = {((atan2(intersection.x, intersection.z) / PI) + 1.f) * .5f,
              -(asin(intersection.y) / PI) + .5f};

    return get4f(mat->map_Kd, st);
}
