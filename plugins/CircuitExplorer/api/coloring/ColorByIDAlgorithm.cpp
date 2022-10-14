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

#include "ColorByIDAlgorithm.h"

#include <brayns/engine/components/ColorTools.h>

std::vector<uint64_t> ColorByIDAlgorithm::execute(
    const std::map<uint64_t, brayns::Vector4f> &colorMap,
    const std::vector<uint64_t> &ids,
    const std::function<void(uint64_t, size_t, const brayns::Vector4f &)> &elementCallback)
{
    if (colorMap.empty())
    {
        return _randomly(ids, elementCallback);
    }

    return _withInput(colorMap, ids, elementCallback);
}

std::vector<uint64_t> ColorByIDAlgorithm::_withInput(
    const std::map<uint64_t, brayns::Vector4f> &colorMap,
    const std::vector<uint64_t> &ids,
    const std::function<void(uint64_t, size_t, const brayns::Vector4f &)> &elementCallback)
{
    auto idBegin = ids.begin();
    auto idEnd = ids.end();
    auto idIt = idBegin;

    auto colorMapBegin = colorMap.begin();
    auto colorMapEnd = colorMap.end();
    auto colorMapIt = colorMapBegin;

    std::vector<uint64_t> skipped;
    skipped.reserve(ids.size());

    while (colorMapIt != colorMapEnd)
    {
        auto inputId = colorMapIt->first;
        auto targetId = *idIt;

        // Fast forward all ids below the first available one
        while (inputId < targetId && colorMapIt != colorMapEnd)
        {
            ++colorMapIt;
            inputId = colorMapIt->first;
        }

        // We have exahusted the input color map, exit
        if (colorMapIt == colorMapEnd)
        {
            return skipped;
        }

        const auto &inputColor = colorMapIt->second;

        // Fast forward to the next cell to color
        while (targetId < inputId && idIt != idEnd)
        {
            skipped.push_back(targetId);
            ++idIt;
            targetId = *idIt;
        }

        // We have reached the end of the available ids to color, exit
        if (idIt == idEnd)
        {
            return skipped;
        }

        // The input id is not present in the element ids
        if (inputId == targetId)
        {
            const auto index = std::distance(idBegin, idIt);
            elementCallback(targetId, index, inputColor);
            ++idIt;
        }

        ++colorMapIt;
    }

    // Add the remaining ids that were not colored (if any)
    while (idIt != idEnd)
    {
        skipped.push_back(*idIt);
        ++idIt;
    }

    skipped.shrink_to_fit();
    return skipped;
}

std::vector<uint64_t> ColorByIDAlgorithm::_randomly(
    const std::vector<uint64_t> &ids,
    const std::function<void(uint64_t, size_t, const brayns::Vector4f &)> &elementCallback)
{
    const auto numCells = ids.size();
    auto roulette = brayns::ColorRoulette();
    for (size_t i = 0; i < numCells; ++i)
    {
        elementCallback(ids[i], i, roulette.getNextColor());
    }

    return {};
}
