/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "SynapseColorHandler.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>

#include <components/CircuitIds.h>

#include <api/coloring/ColorByIDAlgorithm.h>
#include <api/coloring/ColorUtils.h>

namespace
{
class Extractor
{
public:
    static std::vector<brayns::GeometryView> &extractViews(brayns::Components &components)
    {
        auto &views = components.get<brayns::GeometryViews>();
        views.modified = true;
        return views.elements;
    }

    static const std::vector<brayns::Geometry> &extractGeometries(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        return geometries.elements;
    }

    static std::vector<uint64_t> &extractIds(brayns::Components &components)
    {
        auto &ids = components.get<CircuitIds>();
        return ids.elements;
    }
};
}

SynapseColorHandler::SynapseColorHandler(brayns::Components &components)
    : _components(components)
{
}

void SynapseColorHandler::updateColor(const brayns::Vector4f &color)
{
    auto &views = Extractor::extractViews(_components);
    for (auto &view : views)
    {
        view.setColor(color);
    }
}

std::vector<uint64_t> SynapseColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto &views = Extractor::extractViews(_components);
    auto &ids = Extractor::extractIds(_components);

    return ColorByIDAlgorithm::execute(
        colorMap,
        ids,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            views[index].setColor(color);
        });
}

void SynapseColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    auto &views = Extractor::extractViews(_components);
    assert(views.size() == colors.size());
    for (size_t i = 0; i < views.size(); ++i)
    {
        views[i].setColor(colors[i]);
    }
}

void SynapseColorHandler::updateColorByMethod(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    if (!vars.empty())
    {
        _colorWithInput(colorData, method, vars);
    }
    else
    {
        _colorAll(colorData, method);
    }
}

void SynapseColorHandler::updateIndexedColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &indices)
{
    assert(color.size() <= 256);

    auto &geometries = Extractor::extractGeometries(_components);
    auto &views = Extractor::extractViews(_components);

    auto colorData = ospray::cpp::CopiedData(color);

    size_t mappingOffset = 0;
    for (size_t i = 0; i < views.size(); ++i)
    {
        auto geometrySize = geometries[i].numPrimitives();
        assert(mappingOffset + geometrySize <= indices.size());

        auto synapseIndices = &indices[mappingOffset];
        auto mappingData = ospray::cpp::CopiedData(synapseIndices, geometrySize);
        views[i].setColorMap(mappingData, colorData);
    }
}

void SynapseColorHandler::_colorWithInput(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    auto &ids = Extractor::extractIds(_components);
    auto perIdValues = colorData.getMethodValuesForIDs(method, ids);

    std::unordered_map<std::string, brayns::Vector4f> groupedVariables;
    for (const auto &variable : vars)
    {
        const auto &name = variable.variable;
        const auto &color = variable.color;
        groupedVariables[name] = color;
    }

    std::map<uint64_t, brayns::Vector4f> colorMap;
    for (size_t i = 0; i < perIdValues.size(); ++i)
    {
        const auto id = ids[i];
        const auto &value = perIdValues[i];

        auto it = groupedVariables.find(value);
        if (it == groupedVariables.end())
        {
            continue;
        }

        const auto &color = it->second;
        colorMap[id] = color;
    }

    updateColorById(colorMap);
}

void SynapseColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    auto &ids = Extractor::extractIds(_components);
    auto perIdValues = colorData.getMethodValuesForIDs(method, ids);

    std::vector<brayns::Vector4f> result;
    result.reserve(ids.size());

    ColorDeck deck;
    for (auto &value : perIdValues)
    {
        result.push_back(deck.getColorForKey(value));
    }
    updateColorById(result);
}
