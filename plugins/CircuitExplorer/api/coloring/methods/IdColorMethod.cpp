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

#include "IdColorMethod.h"

#include <brayns/engine/common/ExtractColor.h>
#include <brayns/engine/components/GeometryViews.h>

#include <brayns/utils/parsing/Parser.h>

#include <api/utils/RangeBuilder.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>

#include <unordered_map>

namespace
{
class ColorByIDAlgorithm
{
public:
    using ColorMap = std::unordered_map<std::string, brayns::Vector4f>;

    template<typename Callable>
    static void execute(const std::vector<uint64_t> &ids, const ColorMap &colorMap, Callable &&callback)
    {
        auto idColorList = _buildIdColorList(colorMap);

        for (size_t i = 0, j = 0; i < ids.size(); ++i)
        {
            while (j < idColorList.size() && ids[i] > idColorList[j].id)
            {
                ++j;
            }

            if (j == idColorList.size())
            {
                return;
            }

            if (ids[i] < idColorList[j].id)
            {
                continue;
            }

            callback(i, ids[i], idColorList[j].color);
            ++j;
        }
    }

private:
    struct ColorListEntry
    {
        std::vector<uint64_t> ids;
        brayns::Vector4f color;
    };

    struct ColorEntry
    {
        uint64_t id;
        brayns::Vector4f color;
    };

    static std::vector<ColorListEntry> _parseColorMap(const ColorMap &colorMap)
    {
        auto tempColorList = std::vector<ColorListEntry>();
        tempColorList.reserve(colorMap.size());

        for (auto &[range, color] : colorMap)
        {
            auto ids = brayns::Parser::parseRange<uint64_t>(range);
            tempColorList.push_back({std::move(ids), color});
        }

        return tempColorList;
    }

    static size_t _countEntries(const std::vector<ColorListEntry> &entries)
    {
        size_t count = 0;
        for (auto &entry : entries)
        {
            count += entry.ids.size();
        }
        return count;
    }

    static std::vector<ColorEntry> _buildColorList(const std::vector<ColorListEntry> &entries)
    {
        auto colorIdList = std::vector<ColorEntry>();
        colorIdList.reserve(_countEntries(entries));

        for (auto &entry : entries)
        {
            auto &ids = entry.ids;
            for (auto id : ids)
            {
                colorIdList.push_back({id, entry.color});
            }
        }

        return colorIdList;
    }

    static std::vector<ColorEntry> _buildIdColorList(const ColorMap &colorMap)
    {
        auto colorListEntries = _parseColorMap(colorMap);
        auto colorIdList = _buildColorList(colorListEntries);

        std::sort(colorIdList.begin(), colorIdList.end(), [](auto &a, auto &b) { return a.id < b.id; });

        return colorIdList;
    }
};
}

std::string IdColorMethod::getName() const
{
    return "id";
}

std::vector<std::string> IdColorMethod::getValues(brayns::Components &components) const
{
    auto &ids = components.get<CircuitIds>().elements;
    return {RangeBuilder<uint64_t>::build(ids)};
}

void IdColorMethod::apply(brayns::Components &components, const brayns::ColorMethodInput &input) const
{
    auto &ids = components.get<CircuitIds>().elements;
    auto &colors = brayns::ExtractColor::extractList(components, ids.size());

    ColorByIDAlgorithm::execute(
        ids,
        input,
        [&](size_t index, uint64_t id, const brayns::Vector4f &color)
        {
            (void)id;
            colors.elements[index] = color;
        });

    auto &views = components.get<brayns::GeometryViews>();
    auto &painter = *components.get<ColorHandler>().handler;
    painter.colorByElement(colors, views);
}
