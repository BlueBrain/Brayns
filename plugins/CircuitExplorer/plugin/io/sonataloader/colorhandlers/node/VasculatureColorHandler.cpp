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

#include "VasculatureColorHandler.h"

#include <brayns/engine/Material.h>

#include <plugin/api/ColorUtils.h>
#include <plugin/io/morphology/vasculature/VasculatureMaterialMap.h>

namespace sonataloader
{
namespace
{
constexpr char methodBySection[] = "vasculature_section";
} // namespace

void VasculatureColorHandler::_setElementsImpl(
    const std::vector<uint64_t>& ids,
    std::vector<ElementMaterialMap::Ptr>&& elements)
{
    _ids = ids;
    _elements = std::move(elements);
    for (auto& element : _elements)
    {
        auto& vmm = static_cast<VasculatureMaterialMap&>(*element.get());
        _sectionMaterials[vmm.sectionType].push_back(vmm.materialId);
    }
}

std::vector<std::string> VasculatureColorHandler::_getMethodsImpl() const
{
    return {methodBySection};
}

std::vector<std::string> VasculatureColorHandler::_getMethodVariablesImpl(
    const std::string&) const
{
    return brayns::enumNames<VasculatureSection>();
}

void VasculatureColorHandler::_updateColorByIdImpl(
    const std::map<uint64_t, brayns::Vector4f>& colorMap)
{
    if (!colorMap.empty())
    {
        auto it = colorMap.begin();
        size_t i = 0;
        while (it != colorMap.end() && i < _ids.size())
        {
            const auto id = it->first;
            if (id > _ids.back())
                throw std::invalid_argument(
                    "Requested coloring ID '" + std::to_string(id) +
                    "' is beyond the highest ID loaded '" +
                    std::to_string(_ids.back()) + "'");

            while (_ids[i] < id && i < _ids.size())
                ++i;

            if (_ids[i] == id)
                _elements[i]->setColor(_model, it->second);

            ++it;
        }
    }
    else
    {
        ColorRoulette r;
        for (auto& element : _elements)
            element->setColor(_model, r.getNextColor());
    }
}

void VasculatureColorHandler::_updateSingleColorImpl(
    const brayns::Vector4f& color)
{
    for (auto& element : _elements)
        element->setColor(_model, color);
}

void VasculatureColorHandler::_updateColorImpl(
    const std::string&, const std::vector<ColoringInformation>& variables)
{
    if (!variables.empty())
    {
        for (const auto& entry : variables)
        {
            const auto sectionType =
                brayns::stringToEnum<VasculatureSection>(entry.variable);
            auto sectionMaterials = _sectionMaterials.find(sectionType);
            if (sectionMaterials != _sectionMaterials.end())
            {
                for (const auto materialId : sectionMaterials->second)
                    _updateMaterial(materialId, entry.color);
            }
        }
    }
    else
    {
        const std::vector<VasculatureSection> allSections = {
            VasculatureSection::ARTERIAL_CAPILLARY,
            VasculatureSection::ARTERIOLE,
            VasculatureSection::ARTERY,
            VasculatureSection::TRANSITIONAL,
            VasculatureSection::VEIN,
            VasculatureSection::VENOUS_CAPILLARY,
            VasculatureSection::VENULE};

        ColorRoulette roulette;
        for (const auto& section : allSections)
        {
            auto sectionMaterials = _sectionMaterials.find(section);
            if (sectionMaterials != _sectionMaterials.end())
            {
                const auto& color = roulette.getNextColor();
                for (const auto materialId : sectionMaterials->second)
                    _updateMaterial(materialId, color);
            }
        }
    }
}
} // namespace sonataloader
