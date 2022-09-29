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

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>

#include <api/coloring/ColorByIDAlgorithm.h>
#include <api/coloring/ColorUtils.h>
#include <api/neuron/NeuronSection.h>
#include <components/CircuitIds.h>
#include <components/ColorList.h>

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

    static size_t extractNumPrimitives(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        assert(geometries.elements.size() == 1);
        return geometries.elements.front().numPrimitives();
    }

    static std::vector<uint64_t> &extractIds(brayns::Components &components)
    {
        auto &ids = components.get<CircuitIds>();
        return ids.elements;
    }

    static std::vector<brayns::Vector4f> &extractColors(brayns::Components &components)
    {
        auto &colors = components.get<ColorList>();
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
    auto &colors = Extractor::extractColors(_components);
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
    assert(inputColors.size() == Extractor::extractNumPrimitives(_components));

    auto &view = Extractor::extractView(_components);
    auto &colors = Extractor::extractColors(_components);

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
    auto &view = Extractor::extractView(_components);
    view.setColorMap(ospray::cpp::CopiedData(indices), ospray::cpp::CopiedData(color));
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
        for (auto &variable : vars)
        {
            auto &sectionName = variable.variable;
            auto &sectionColor = variable.color;
            auto sectionType = brayns::EnumInfo::getValue<NeuronSection>(sectionName);
            if (sectionType == NeuronSection::Soma)
            {
                updateColor(sectionColor);
            }
        }
    }
    else
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
            if (it != groupedVariables.end())
            {
                colorMap[id] = it->second;
            }
        }

        updateColorById(colorMap);
    }
}

void SomaColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    const auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    if (methodEnum == NeuronColorMethod::ByMorphologySection)
    {
        ColorRoulette roulette;
        updateColor(roulette.getNextColor());
    }
    else
    {
        ColorDeck deck;
        auto &ids = Extractor::extractIds(_components);
        auto perIdValues = colorData.getMethodValuesForIDs(method, ids);
        std::vector<brayns::Vector4f> result(ids.size());
        for (size_t i = 0; i < ids.size(); ++i)
        {
            const auto &value = perIdValues[i];
            const auto &color = deck.getColorForKey(value);
            result[i] = color;
        }
        updateColorById(result);
    }
}
