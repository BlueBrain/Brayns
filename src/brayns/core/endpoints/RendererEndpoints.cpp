/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "RendererEndpoints.h"

#include <brayns/core/objects/RendererObjects.h>

namespace brayns
{
void addRendererEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("getRenderer", [&](GetObjectParams params) { return getRenderer(objects, params); }).description("Get a renderer of any type");
    builder.endpoint("updateRenderer", [&](UpdateRendererParams params) { updateRenderer(objects, device, params); })
        .description("Update a renderer of any type");

    builder.endpoint("createAoRenderer", [&](CreateAoRendererParams params) { return createAoRenderer(objects, device, params); })
        .description("Create a new renderer based on ambient occlusion only (ignore lights)");
    builder.endpoint("getAoRenderer", [&](GetObjectParams params) { return getAoRenderer(objects, params); })
        .description("Get AO renderer specific params");
    builder.endpoint("updateAoRenderer", [&](UpdateAoRendererParams params) { updateAoRenderer(objects, device, params); })
        .description("Update AO renderer specific params");

    builder.endpoint("createScivisRenderer", [&](CreateScivisRendererParams params) { return createScivisRenderer(objects, device, params); })
        .description("Create a new scivis renderer for scientific visualization (use phong shading)");
    builder.endpoint("getScivisRenderer", [&](GetObjectParams params) { return getScivisRenderer(objects, params); })
        .description("Get scivis renderer specific params");
    builder.endpoint("updateScivisRenderer", [&](UpdateScivisRendererParams params) { updateScivisRenderer(objects, device, params); })
        .description("Update scivis renderer specific params");

    builder.endpoint("createPathTracer", [&](CreatePathTracerParams params) { return createPathTracer(objects, device, params); })
        .description("Create a new path tracer for photo-realistic renderings");
    builder.endpoint("getPathTracer", [&](GetObjectParams params) { return getPathTracer(objects, params); })
        .description("Get path tracer specific params");
    builder.endpoint("updatePathTracer", [&](UpdatePathTracerParams params) { updatePathTracer(objects, device, params); })
        .description("Update path tracer specific params");
}
}
