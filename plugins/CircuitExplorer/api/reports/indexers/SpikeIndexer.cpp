/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "SpikeIndexer.h"

#include <numeric>

namespace
{
class CellSubElementCount
{
public:
    static std::vector<size_t> fromCellComparments(const std::vector<CellCompartments> &cellCompartments)
    {
        std::vector<size_t> cellSubElements;
        cellSubElements.reserve(cellCompartments.size());
        for (auto &comparment : cellCompartments)
        {
            cellSubElements.push_back(comparment.numItems);
        }
        return cellSubElements;
    }
};
}

SpikeIndexer::SpikeIndexer(const std::vector<CellCompartments> &cellCompartments):
    _cellSubElements(CellSubElementCount::fromCellComparments(cellCompartments)),
    _totalElements(std::accumulate(_cellSubElements.begin(), _cellSubElements.end(), 0))
{
}

std::vector<uint8_t> SpikeIndexer::generate(const std::vector<float> &data, const brayns::Vector2f &range) noexcept
{
    // Spike values are hardcoed in range 0 - 1
    (void)range;

    std::vector<uint8_t> indices(_totalElements, 0);
    size_t writeIndex = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        auto value = data[i];
        auto index = static_cast<uint8_t>(value * 255);

        auto writeCount = _cellSubElements[i];
        auto begin = indices.begin();
        std::advance(begin, writeIndex);
        auto end = indices.begin();
        std::advance(end, writeIndex + writeCount);
        std::fill(begin, end, index);
        writeIndex += writeCount;
    }

    return indices;
}
