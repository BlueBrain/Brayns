/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/log.h>

#include <brayns/engine/Model.h>

#include <common/types.h>

struct ShapeMaterialInfo
{
    size_t id = 0;
    brayns::Vector3d color{0.0};
    double opacity = 1.0;
};

class ShapeMaterial
{
public:
    static void create(brayns::Model& model, const ShapeMaterialInfo& info)
    {
        // Extract info
        auto id = info.id;
        auto& color = info.color;
        auto opacity = info.opacity;

        // Create material instance
        auto material = model.createMaterial(id, std::to_string(id));
        material->setDiffuseColor(color);
        material->setOpacity(opacity);
        material->setSpecularExponent(20.0);

        // Material extra properties
        brayns::PropertyMap properties;
        properties.add({MATERIAL_PROPERTY_CAST_USER_DATA, false});
        properties.add({MATERIAL_PROPERTY_SHADING_MODE,
                        static_cast<int>(MaterialShadingMode::diffuse)});
        properties.add({MATERIAL_PROPERTY_CLIPPING_MODE,
                        static_cast<int>(MaterialClippingMode::no_clipping)});

        // Add extra properties
        material->updateProperties(properties);

        // Commit
        material->markModified();
        material->commit();
    }
};