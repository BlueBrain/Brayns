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
#include "VasculatureColorMethods.h"

#include <api/coloring/ColorUtils.h>

VasculatureColorHandler::VasculatureColorHandler(VasculatureComponent &vasculature)
    : _vasculature(vasculature)
{
}

void VasculatureColorHandler::updateColor(const brayns::Vector4f &color)
{
    _vasculature.setColor(color);
}

std::vector<uint64_t> VasculatureColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    return _vasculature.setColorById(colorMap);
}

void VasculatureColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    _vasculature.setColorById(colors);
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

void VasculatureColorHandler::updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices)
{
    _vasculature.setSimulationColor(color, indices);
}

void VasculatureColorHandler::_colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars)
{
    auto methodEnum = brayns::stringToEnum<VasculatureColorMethods>(method);
    if (methodEnum != VasculatureColorMethods::BY_SECTION)
    {
        return;
    }

    std::vector<std::pair<VasculatureSection, brayns::Vector4f>> sectionColorMap;
    sectionColorMap.reserve(vars.size());

    for (const auto &variable : vars)
    {
        const auto &sectionName = variable.variable;
        const auto &sectionColor = variable.color;
        auto sectionType = brayns::stringToEnum<VasculatureSection>(sectionName);
        sectionColorMap.push_back(std::make_pair(sectionType, sectionColor));
    }

    _vasculature.setColorBySection(sectionColorMap);
}

void VasculatureColorHandler::_colorAll(const std::string &method)
{
    auto methodEnum = brayns::stringToEnum<VasculatureColorMethods>(method);
    if (methodEnum != VasculatureColorMethods::BY_SECTION)
    {
        return;
    }

    ColorRoulette roulette;
    const std::vector<std::pair<VasculatureSection, brayns::Vector4f>> sectionColorMap = {
        std::make_pair(VasculatureSection::ARTERY, roulette.getNextColor()),
        std::make_pair(VasculatureSection::VEIN, roulette.getNextColor()),
        std::make_pair(VasculatureSection::ARTERIOLE, roulette.getNextColor()),
        std::make_pair(VasculatureSection::VENULE, roulette.getNextColor()),
        std::make_pair(VasculatureSection::ARTERIAL_CAPILLARY, roulette.getNextColor()),
        std::make_pair(VasculatureSection::VENOUS_CAPILLARY, roulette.getNextColor()),
        std::make_pair(VasculatureSection::TRANSITIONAL, roulette.getNextColor())};
    _vasculature.setColorBySection(sectionColorMap);
}
