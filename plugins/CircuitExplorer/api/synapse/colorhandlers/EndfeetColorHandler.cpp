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

#include "EndfeetColorHandler.h"

#include <brayns/common/ColorTools.h>
#include <brayns/engine/components/GeometryViews.h>

#include <api/coloring/ColorByIDAlgorithm.h>
#include <components/CircuitIds.h>

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

    static std::vector<uint64_t> &extractIds(brayns::Components &components)
    {
        auto &ids = components.get<CircuitIds>();
        return ids.elements;
    }
};
}

EndfeetColorHandler::EndfeetColorHandler(brayns::Components &components)
    : _components(components)
{
}

void EndfeetColorHandler::updateColor(const brayns::Vector4f &color)
{
    auto &views = Extractor::extractViews(_components);
    for (auto &view : views)
    {
        view.setColor(color);
    }
}

std::vector<uint64_t> EndfeetColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
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

void EndfeetColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    auto &views = Extractor::extractViews(_components);
    assert(views.size() == colors.size());
    for (size_t i = 0; i < views.size(); ++i)
    {
        views[i].setColor(colors[i]);
    }
}

void EndfeetColorHandler::updateColorByMethod(
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

void EndfeetColorHandler::updateIndexedColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &indices)
{
    (void)color;
    (void)indices;
    throw std::runtime_error("No support to color endfeet by indexed colormap");
}

void EndfeetColorHandler::_colorWithInput(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    auto &ids = Extractor::extractIds(_components);
    auto perIdValues = colorData.getMethodValuesForIDs(method, ids);

    std::unordered_map<std::string, brayns::Vector4f> groupedVariables;
    for (const auto &variable : vars)
    {
        auto &name = variable.variable;
        auto &color = variable.color;
        groupedVariables[name] = color;
    }

    std::map<uint64_t, brayns::Vector4f> colorMap;
    for (size_t i = 0; i < perIdValues.size(); ++i)
    {
        auto id = ids[i];
        auto &value = perIdValues[i];

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

void EndfeetColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    auto &ids = Extractor::extractIds(_components);
    auto perIdValues = colorData.getMethodValuesForIDs(method, ids);

    std::vector<brayns::Vector4f> result;
    result.reserve(ids.size());

    auto deck = brayns::ColorDeck();
    for (auto &value : perIdValues)
    {
        result.push_back(deck.getColorForKey(value));
    }
    updateColorById(result);
}
