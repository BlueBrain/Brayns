/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "MaterialUtils.h"

#include <brayns/engine/Material.h>

size_t CircuitExplorerMaterial::create(brayns::Model& model,
                                       const brayns::Vector3f& color,
                                       const float opacity)
{
    const auto matId = model.getMaterials().size();
    auto matPtr = model.createMaterial(matId, "", _getExtraAttributes());
    matPtr->setDiffuseColor(color);
    matPtr->setOpacity(opacity);
    return matId;
}

size_t CircuitExplorerMaterial::createUnlit(brayns::Model& model,
                                            const brayns::Vector3f& color,
                                            const float opacity)
{
    const auto matId = model.getMaterials().size();
    auto matPtr = model.createMaterial(matId, "", _getExtraAttributes());
    matPtr->setDiffuseColor(color);
    matPtr->setOpacity(opacity);
    matPtr->updateProperty(std::string(MATERIAL_PROPERTY_SHADING_MODE),
                           static_cast<int32_t>(MaterialShadingMode::none));
    return matId;
}

void CircuitExplorerMaterial::addExtraAttributes(brayns::Model& model)
{
    for (auto& matEntry : model.getMaterials())
    {
        matEntry.second->updateProperties(_getExtraAttributes());
        matEntry.second->markModified();
        matEntry.second->commit();
    }
}

void CircuitExplorerMaterial::setSimulationColorEnabled(brayns::Model& model,
                                                        const bool value)
{
    for (auto& matEntry : model.getMaterials())
    {
        if (!matEntry.second->hasProperty(
                std::string(MATERIAL_PROPERTY_CAST_USER_DATA)))
            continue;
        matEntry.second->updateProperty(std::string(
                                            MATERIAL_PROPERTY_CAST_USER_DATA),
                                        value);
        matEntry.second->markModified();
        matEntry.second->commit();
    }
}

const brayns::PropertyMap&
    CircuitExplorerMaterial::_getExtraAttributes() noexcept
{
    static brayns::PropertyMap extra;
    if (extra.empty())
    {
        extra.add({std::string(MATERIAL_PROPERTY_CAST_USER_DATA), false});
        extra.add({std::string(MATERIAL_PROPERTY_SHADING_MODE),
                   static_cast<int32_t>(MaterialShadingMode::diffuse)});
        extra.add({std::string(MATERIAL_PROPERTY_CLIPPING_MODE),
                   static_cast<int>(MaterialClippingMode::no_clipping)});
        extra.add({std::string(MATERIAL_PROPERTY_USER_PARAMETER), 1.0});
    }
    return extra;
}
