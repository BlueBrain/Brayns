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

#include "MorphologyColorHandler.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>

#include <api/coloring/ColorByIDAlgorithm.h>
#include <api/coloring/ColorUtils.h>
#include <api/neuron/NeuronColorMethod.h>
#include <api/neuron/NeuronSection.h>
#include <components/CircuitIds.h>
#include <components/NeuronSectionList.h>

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

    static const std::vector<std::vector<NeuronSectionMapping>> &extractNeuronSections(brayns::Components &components)
    {
        auto &sections = components.get<NeuronSectionList>();
        return sections.mappings;
    }
};

class ColorMapPainter
{
public:
    static void paint(
        brayns::Components &components,
        const std::vector<uint8_t> &indices,
        const std::vector<brayns::Vector4f> &colors)
    {
        assert(colors.size() <= 256);

        auto &views = Extractor::extractViews(components);
        auto &geometries = Extractor::extractGeometries(components);

        auto colorData = ospray::cpp::CopiedData(colors);

        size_t mappingOffset = 0;
        for (size_t i = 0; i < views.size(); ++i)
        {
            auto numGeomtries = geometries[i].numPrimitives();
            assert(mappingOffset + numGeomtries <= indices.size());

            auto morphologyMapping = &indices[mappingOffset];
            auto mappingData = ospray::cpp::CopiedData(morphologyMapping, numGeomtries);

            views[i].setColorMap(mappingData, colorData);
            mappingOffset += numGeomtries;
        }
    }
};

struct SectionColor
{
    NeuronSection section;
    brayns::Vector4f color;
};

class SectionPainter
{
public:
    static void paint(brayns::Components &components, const std::vector<SectionColor> &sectionColor)
    {
        std::vector<std::pair<NeuronSection, uint8_t>> sectionIndices;
        sectionIndices.reserve(sectionColor.size());

        std::vector<brayns::Vector4f> colors;
        colors.reserve(sectionColor.size());

        // Crate a color map out of the section colors
        for (const auto &entry : sectionColor)
        {
            const auto index = static_cast<uint8_t>(colors.size());
            colors.push_back(entry.color);
            sectionIndices.push_back(std::make_pair(entry.section, index));
        }

        auto colorData = ospray::cpp::CopiedData(colors);

        // Apply the appropiate color index to the appropiate section on each morphology
        auto &views = Extractor::extractViews(components);
        auto &geometries = Extractor::extractGeometries(components);
        auto &sections = Extractor::extractNeuronSections(components);
        for (size_t i = 0; i < views.size(); ++i)
        {
            std::vector<uint8_t> indices(geometries[i].numPrimitives(), 0u);
            for (const auto &entry : sectionIndices)
            {
                auto sectionType = entry.first;
                auto index = entry.second;

                for (auto &section : sections[i])
                {
                    if (section.type == sectionType)
                    {
                        auto start = indices.begin() + section.begin;
                        auto end = indices.begin() + section.end;
                        std::fill(start, end, index);
                    }
                }
            }
            views[i].setColorMap(ospray::cpp::CopiedData(indices), colorData);
        }
    }
};
}

MorphologyColorHandler::MorphologyColorHandler(brayns::Components &components)
    : _components(components)
{
}

void MorphologyColorHandler::updateColor(const brayns::Vector4f &color)
{
    auto &views = Extractor::extractViews(_components);
    for (auto &view : views)
    {
        view.setColor(color);
    }
}

std::vector<uint64_t> MorphologyColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
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

void MorphologyColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    auto &views = Extractor::extractViews(_components);
    assert(views.size() == colors.size());
    for (size_t i = 0; i < views.size(); ++i)
    {
        views[i].setColor(colors[i]);
    }
}

void MorphologyColorHandler::updateColorByMethod(
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

void MorphologyColorHandler::updateIndexedColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &indices)
{
    ColorMapPainter::paint(_components, indices, color);
}

void MorphologyColorHandler::_colorWithInput(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    const auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    if (methodEnum == NeuronColorMethod::ByMorphologySection)
    {
        std::vector<SectionColor> sectionColorMap;
        sectionColorMap.reserve(vars.size());

        for (const auto &variable : vars)
        {
            const auto &sectionName = variable.variable;
            const auto &sectionColor = variable.color;
            auto sectionType = brayns::EnumInfo::getValue<NeuronSection>(sectionName);
            sectionColorMap.push_back({sectionType, sectionColor});
        }
        SectionPainter::paint(_components, sectionColorMap);
    }
    else
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
}

void MorphologyColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    const auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    if (methodEnum == NeuronColorMethod::ByMorphologySection)
    {
        ColorRoulette roulette;
        std::vector<SectionColor> sectionColorMap{
            {NeuronSection::Soma, roulette.getNextColor()},
            {NeuronSection::Axon, roulette.getNextColor()},
            {NeuronSection::Dendrite, roulette.getNextColor()},
            {NeuronSection::ApicalDendrite, roulette.getNextColor()}};
        SectionPainter::paint(_components, sectionColorMap);
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
