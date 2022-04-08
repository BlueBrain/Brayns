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

#include <api/coloring/ColorUtils.h>
#include <api/neuron/NeuronColorMethods.h>

MorphologyColorHandler::MorphologyColorHandler(MorphologyCircuitComponent &circuit)
    : _circuit(circuit)
{
}

void MorphologyColorHandler::updateColor(const brayns::Vector4f &color)
{
    _circuit.setColor(color);
}

void MorphologyColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    _circuit.setColorById(colorMap);
}

void MorphologyColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    _circuit.setColorById(colors);
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

void MorphologyColorHandler::updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices)
{
    _circuit.setIndexedColor(color, indices);
}

void MorphologyColorHandler::_colorWithInput(
    const IColorData &colorData,
    const std::string &method,
    const std::vector<ColoringInformation> &vars)
{
    const auto methodEnum = brayns::stringToEnum<NeuronColorMethods>(method);
    if (methodEnum == NeuronColorMethods::BY_MORPHOLOGY_SECTION)
    {
        std::vector<std::pair<NeuronSection, brayns::Vector4f>> sectionColorMap;
        sectionColorMap.reserve(vars.size());

        for (const auto &variable : vars)
        {
            const auto &sectionName = variable.variable;
            const auto &sectionColor = variable.color;
            auto sectionType = brayns::stringToEnum<NeuronSection>(sectionName);
            sectionColorMap.push_back(std::make_pair(sectionType, sectionColor));
        }

        _circuit.setColorBySection(sectionColorMap);
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

void MorphologyColorHandler::_colorAll(const IColorData &colorData, const std::string &method)
{
    const auto methodEnum = brayns::stringToEnum<NeuronColorMethods>(method);
    if (methodEnum == NeuronColorMethods::BY_MORPHOLOGY_SECTION)
    {
        ColorRoulette roulette;
        const std::vector<std::pair<NeuronSection, brayns::Vector4f>> sectionColorMap = {
            std::make_pair(NeuronSection::SOMA, roulette.getNextColor()),
            std::make_pair(NeuronSection::AXON, roulette.getNextColor()),
            std::make_pair(NeuronSection::DENDRITE, roulette.getNextColor()),
            std::make_pair(NeuronSection::APICAL_DENDRITE, roulette.getNextColor())};
        _circuit.setColorBySection(sectionColorMap);
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
