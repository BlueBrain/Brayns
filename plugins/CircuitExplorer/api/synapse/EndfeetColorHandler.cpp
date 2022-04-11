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

#include <api/coloring/ColorUtils.h>

EndfeetColorHandler::EndfeetColorHandler(EndfeetComponent &endfeet)
    : _endfeet(endfeet)
{
}

void EndfeetColorHandler::updateColor(const brayns::Vector4f &color)
{
    _endfeet.setColor(color);
}

void EndfeetColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    _endfeet.setColorById(colorMap);
}

void EndfeetColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    _endfeet.setColorById(colors);
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

void EndfeetColorHandler::updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices)
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
    const auto &ids = _endfeet.getAstroctyeIds();
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

    _endfeet.setColorById(colorMap);
}

void EndfeetColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    ColorDeck deck;
    const auto &ids = _endfeet.getAstroctyeIds();
    const auto perIdValues = colorData.getMethodValuesForIDs(method, ids);
    std::vector<brayns::Vector4f> result(ids.size());
    for (size_t i = 0; i < ids.size(); ++i)
    {
        const auto &value = perIdValues[i];
        const auto &color = deck.getColorForKey(value);
        result[i] = color;
    }
    _endfeet.setColorById(result);
}