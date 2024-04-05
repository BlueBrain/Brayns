/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "MorphologySectionTypeColorMethod.h"

#include <brayns/core/engine/common/ExtractColor.h>
#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/components/GeometryViews.h>

#include <brayns/circuit/api/coloring/handlers/ComposedColorHandler.h>
#include <brayns/circuit/components/ColorHandler.h>
#include <brayns/circuit/components/NeuronSectionType.h>

#include <algorithm>

#include <spdlog/fmt/fmt.h>

namespace
{
struct SectionColor
{
    NeuronSection section;
    brayns::Vector4f color;
};

class SectionPainter
{
public:
    static void paint(brayns::Components &components, const std::vector<SectionColor> &sectionColors)
    {
        auto &views = components.get<brayns::GeometryViews>();
        auto &geometries = components.get<brayns::Geometries>();
        auto &painter = *components.get<ColorHandler>().handler;
        auto &colorMap = _buildColorMap(components, sectionColors);
        painter.colorByColormap(colorMap, geometries, views);
    }

private:
    static brayns::ColorMap &_buildColorMap(
        brayns::Components &components,
        const std::vector<SectionColor> &sectionColors)
    {
        auto sectionIndices = _indexSectionColors(sectionColors);

        auto &colorMap = brayns::ExtractColor::extractMap(components);

        colorMap.colors = _buildColorBuffer(sectionColors);

        auto &geometries = components.get<brayns::Geometries>().elements;
        auto &sections = components.get<NeuronSectionType>().mappings;
        colorMap.indices = _buildIndexBuffer(geometries, sections, sectionIndices);

        return colorMap;
    }

    struct IndexedSection
    {
        uint8_t index;
        NeuronSection section;
    };

    static std::vector<IndexedSection> _indexSectionColors(const std::vector<SectionColor> &sectionColor)
    {
        auto sectionIndices = std::vector<IndexedSection>();
        sectionIndices.reserve(sectionColor.size());

        for (size_t i = 0; i < sectionColor.size(); ++i)
        {
            auto index = static_cast<uint8_t>(i);
            auto section = sectionColor[i].section;
            sectionIndices.push_back({index, section});
        }

        return sectionIndices;
    }

    static std::vector<brayns::Vector4f> _buildColorBuffer(const std::vector<SectionColor> &sectionColors)
    {
        auto result = std::vector<brayns::Vector4f>();
        result.reserve(sectionColors.size());

        for (auto &section : sectionColors)
        {
            result.push_back(section.color);
        }

        return result;
    }

    static std::vector<uint8_t> _buildIndexBuffer(
        const std::vector<brayns::Geometry> &geometries,
        const std::vector<std::vector<SectionTypeMapping>> &sections,
        const std::vector<IndexedSection> &sectionIndices)
    {
        auto indices = std::vector<uint8_t>(_countPrimitives(geometries));

        auto elementIndexBegin = 0ul;

        for (size_t i = 0; i < geometries.size(); ++i)
        {
            for (auto &entry : sectionIndices)
            {
                auto &mapping = _getMappingForSection(entry.section, sections[i]);

                auto start = indices.begin() + elementIndexBegin + mapping.begin;
                auto end = indices.begin() + elementIndexBegin + mapping.end;
                std::fill(start, end, entry.index);
            }

            elementIndexBegin += geometries[i].numPrimitives();
        }

        return indices;
    }

    static size_t _countPrimitives(const std::vector<brayns::Geometry> &geometries)
    {
        auto totalPrimitives = 0ul;

        for (auto &geometry : geometries)
        {
            totalPrimitives += geometry.numPrimitives();
        }

        return totalPrimitives;
    }

    static const SectionTypeMapping &_getMappingForSection(
        NeuronSection section,
        const std::vector<SectionTypeMapping> &mapping)
    {
        auto it = std::find_if(
            mapping.begin(),
            mapping.end(),
            [&](auto &sectionMapping) { return sectionMapping.type == section; });

        if (it == mapping.end())
        {
            auto name = brayns::EnumInfo::getName(section);
            throw std::invalid_argument(fmt::format("Neuron section '{}' not present in geometry", name));
        }

        return *it;
    }
};
}

std::string MorphologySectionTypeColorMethod::getName() const
{
    return "morphology section";
}

std::vector<std::string> MorphologySectionTypeColorMethod::getValues(brayns::Components &components) const
{
    auto &sections = components.get<NeuronSectionType>().mappings;
    auto &element = sections.front();

    auto result = std::vector<std::string>();
    result.reserve(element.size());

    for (auto &entry : element)
    {
        result.push_back(brayns::EnumInfo::getName(entry.type));
    }

    std::sort(result.begin(), result.end());
    return result;
}

void MorphologySectionTypeColorMethod::apply(brayns::Components &components, const brayns::ColorMethodInput &input) const
{
    std::vector<SectionColor> sectionColorMap;
    sectionColorMap.reserve(input.size());

    for (auto &[name, color] : input)
    {
        auto section = brayns::EnumInfo::getValue<NeuronSection>(name);
        sectionColorMap.push_back({section, color});
    }

    SectionPainter::paint(components, sectionColorMap);
}
