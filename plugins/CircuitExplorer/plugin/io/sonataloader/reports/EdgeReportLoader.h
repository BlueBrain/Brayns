/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <bbp/sonata/population.h>

namespace sonataloader
{
/**
 * @brief The EdgeSimulationMapping struct holds the simulation mapping
 * information of all the edges of a single cell. The mapping is given as the
 * simulation array offset for each edge id.
 */
struct EdgeReportMapping
{
    std::unordered_map<bbp::sonata::EdgeID, uint64_t> offsets;
};

/**
 * @brief The EdgeReportLoader class is the base class to manage SONATA report
 * for edge populations. Is in charge of computing the mapping for a set of
 * edges
 */
class EdgeReportLoader
{
public:
    using Ptr = std::unique_ptr<EdgeReportLoader>;

    virtual ~EdgeReportLoader() = default;

    /**
     * @brief computes and returns the simulation mapping for the set of given
     *        cells
     */
    virtual std::vector<EdgeReportMapping> loadMapping(
        const std::string& reportPath, const std::string& population,
        const bbp::sonata::Selection&) const = 0;
};
} // namespace sonataloader
