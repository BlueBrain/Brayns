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
#include <vector>

#include <bbp/sonata/population.h>

namespace sonataloader
{
/**
 * @brief The NodeSimulationMapping struct holds information about a single
 * cells simulation mapping. The mapping is given as a global offset into the
 * simulation array, plus local offsets and number of compartments for each
 * reported section
 */
struct NodeReportMapping
{
    size_t globalOffset;
    std::vector<uint16_t> offsets;
    std::vector<uint16_t> compartments;
};

/**
 * @brief The NodeReportLoader class is the base class to manage SONATA report
 * for node populations. Is in charge of computing the mapping for a set of
 * nodes
 */
class NodeReportLoader
{
public:
    using Ptr = std::unique_ptr<NodeReportLoader>;

    virtual ~NodeReportLoader() = default;

    const std::string& getPopulationName() const noexcept;

    /**
     * @brief computes and returns the simulation mapping for the set of given
     *        cells
     */
    virtual std::vector<NodeReportMapping> loadMapping(
        const std::string& reportPath, const std::string& population,
        const bbp::sonata::Selection&) const = 0;
};
} // namespace sonataloader
