/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "SomaColorHandler.h"

#include <brayns/engine/components/GeometryViews.h>

#include <api/coloring/ColorByIDAlgorithm.h>
#include <api/coloring/ColorUtils.h>
#include <api/neuron/NeuronSection.h>
#include <components/CircuitIds.h>
#include <components/SomaColors.h>

namespace
{
class Extractor
{
public:
    static brayns::GeometryView &extractView(brayns::Components &components)
    {
        auto &views = components.get<brayns::GeometryViews>();
        assert(views.elements.size() == 1);
        views.modified = true;
        return views.elements.back();
    }

    static std::vector<uint64_t> &extractIds(brayns::Components &components)
    {
        auto &ids = components.get<CircuitIds>();
        return ids.elements;
    }

    static std::vector<brayns::Vector4f> &extractSomaColors(brayns::Components &components)
    {
        auto &colors = components.get<SomaColors>();
        return colors.elements;
    }
};
}

SomaColorHandler::SomaColorHandler(brayns::Components &components)
    : _components(components)
{
}

void SomaColorHandler::updateColor(const brayns::Vector4f &color)
{
    auto view = Extractor::extractView(_components);
    view.setColor(color);
}

std::vector<uint64_t> SomaColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto &view = Extractor::extractView(_components);
    auto &ids = Extractor::extractIds(_components);
    auto &colors = Extractor::extractSomaColors(_components);
    auto nonColoredIds = ColorByIDAlgorithm::execute(
        colorMap,
        ids,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            colors[index] = color;
        });
    view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
    return nonColoredIds;
}

void SomaColorHandler::updateColorById(const std::vector<brayns::Vector4f> &inputColors)
{
    auto &view = Extractor::extractView(_components);
    auto &colors = Extractor::extractSomaColors(_components);

    colors = inputColors;
    view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
}

void SomaColorHandler::updateColorByMethod(
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

void SomaColorHandler::updateIndexedColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &indices)
{
    _circuit.setIndexedColor(color, indices);
}

void SomaColorHandler::_colorWithInput(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    const auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    if (methodEnum == NeuronColorMethod::ByMorphologySection)
    {
        // Search for soma
        for (const auto &variable : vars)
        {
            const auto &sectionName = variable.variable;
            const auto &sectionColor = variable.color;
            auto sectionType = brayns::EnumInfo::getValue<NeuronSection>(sectionName);
            if (sectionType == NeuronSection::Soma)
            {
                _circuit.setColor(sectionColor);
                break;
            }
        }
    }
    else
    {
        const auto &ids = _circuit.getIDs();
        const auto perIdValues = colorData.getMethodValuesForIDs(method, ids);

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

        _circuit.setColorById(colorMap);
    }
}

void SomaColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    const auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    if (methodEnum == NeuronColorMethod::ByMorphologySection)
    {
        ColorRoulette roulette;
        _circuit.setColor(roulette.getNextColor());
    }
    else
    {
        ColorDeck deck;
        const auto &ids = _circuit.getIDs();
        const auto perIdValues = colorData.getMethodValuesForIDs(method, ids);
        std::vector<brayns::Vector4f> result(ids.size());
        for (size_t i = 0; i < ids.size(); ++i)
        {
            const auto &value = perIdValues[i];
            const auto &color = deck.getColorForKey(value);
            result[i] = color;
        }
        _circuit.setColorById(result);
    }
}
