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

#include "EndFootColorHandler.h"

#include <plugin/api/ColorUtils.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

namespace sonataloader
{
void EndFootColorHandler::_setElementsImpl(
    const std::vector<uint64_t>&,
    std::vector<ElementMaterialMap::Ptr>&& elements)
{
    size_t totalMaterials = 0;
    for (const auto& element : elements)
    {
        const auto& emm =
            static_cast<const SynapseMaterialMap&>(*element.get());
        totalMaterials += emm.materials.size();
    }

    _materials.reserve(totalMaterials);
    for (const auto& element : elements)
    {
        const auto& emm =
            static_cast<const SynapseMaterialMap&>(*element.get());
        for (const auto& material : emm.materials)
            _materials.push_back(material.material);
    }
}

void EndFootColorHandler::_updateColorByIdImpl(
    const std::map<uint64_t, brayns::Vector4f>&)
{
    ColorRoulette r;
    for (const auto matId : _materials)
        _updateMaterial(matId, r.getNextColor());
}

void EndFootColorHandler::_updateSingleColorImpl(const brayns::Vector4f& color)
{
    for (const auto matId : _materials)
        _updateMaterial(matId, color);
}
} // namespace sonataloader
