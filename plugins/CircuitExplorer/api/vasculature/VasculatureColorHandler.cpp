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

#include "VasculatureColorHandler.h"

#include "VasculatureColorMethod.h"
#include "VasculatureSection.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/ColorTools.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>

#include <api/coloring/ColorByIDAlgorithm.h>
#include <api/neuron/NeuronSection.h>
#include <components/CircuitIds.h>
#include <components/ColorList.h>
#include <components/VasculatureSectionList.h>

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

    static std::vector<VasculatureSection> &extractSections(brayns::Components &components)
    {
        auto &sections = components.get<VasculatureSectionList>();
        return sections.sections;
    }
};

struct SectionColor
{
    VasculatureSection section;
    brayns::Vector4f color;
};

class SectionPainter
{
public:
    static void paint(brayns::Components &components, const std::vector<SectionColor> &colorMap)
    {
        auto &colors = Extractor::extractColors(components);
        auto &sections = Extractor::extractSections(components);

        bool somethingColored = false;
        for (size_t i = 0; i < colors.size(); ++i)
        {
            auto &section = sections[i];
            for (auto &entry : colorMap)
            {
                if (entry.section == section)
                {
                    colors[i] = entry.color;
                    somethingColored = true;
                }
            }
        }
        if (somethingColored)
        {
            auto &view = Extractor::extractView(components);
            view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
        }
    }
};
}

VasculatureColorHandler::VasculatureColorHandler(brayns::Components &components)
    : _components(components)
{
}

void VasculatureColorHandler::updateColor(const brayns::Vector4f &color)
{
    auto &view = Extractor::extractView(_components);
    view.setColor(color);
}

std::vector<uint64_t> VasculatureColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
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

void VasculatureColorHandler::updateColorById(const std::vector<brayns::Vector4f> &inputColors)
{
    assert(inputColors.size() == Extractor::extractNumPrimitives(_components));
    auto &view = Extractor::extractView(_components);
    auto &colors = Extractor::extractColors(_components);
    colors = inputColors;
    view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
}

void VasculatureColorHandler::updateColorByMethod(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    (void)colorData;

    if (!vars.empty())
    {
        _colorWithInput(method, vars);
    }
    else
    {
        _colorAll(method);
    }
}

void VasculatureColorHandler::updateIndexedColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &indices)
{
    auto &view = Extractor::extractView(_components);
    view.setColorMap(ospray::cpp::CopiedData(indices), ospray::cpp::CopiedData(color));
}

void VasculatureColorHandler::_colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars)
{
    auto methodEnum = brayns::EnumInfo::getValue<VasculatureColorMethod>(method);
    if (methodEnum != VasculatureColorMethod::BySection)
    {
        return;
    }

    std::vector<SectionColor> sectionColorMap;
    sectionColorMap.reserve(vars.size());

    for (auto &variable : vars)
    {
        auto &sectionName = variable.variable;
        auto &sectionColor = variable.color;
        auto sectionType = brayns::EnumInfo::getValue<VasculatureSection>(sectionName);
        sectionColorMap.push_back({sectionType, sectionColor});
    }
    SectionPainter::paint(_components, sectionColorMap);
}

void VasculatureColorHandler::_colorAll(const std::string &method)
{
    auto methodEnum = brayns::EnumInfo::getValue<VasculatureColorMethod>(method);
    if (methodEnum != VasculatureColorMethod::BySection)
    {
        return;
    }

    auto roulette = brayns::ColorRoulette();
    std::vector<SectionColor> sectionColorMap = {
        {VasculatureSection::Artery, roulette.getNextColor()},
        {VasculatureSection::Vein, roulette.getNextColor()},
        {VasculatureSection::Arteriole, roulette.getNextColor()},
        {VasculatureSection::Venule, roulette.getNextColor()},
        {VasculatureSection::ArterialCapillary, roulette.getNextColor()},
        {VasculatureSection::VenousCapillary, roulette.getNextColor()},
        {VasculatureSection::Transitional, roulette.getNextColor()}};
    SectionPainter::paint(_components, sectionColorMap);
}
