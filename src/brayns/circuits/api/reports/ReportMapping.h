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

#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <brayns/circuits/api/neuron/NeuronGeometry.h>

/**
 * @brief The CellCompartments represents the internal structure of an element onto which simulation will be mapped
 */
struct CellCompartments
{
    size_t numItems{};
    std::vector<SectionSegmentMapping> sectionSegments;
};

/**
 * @brief The CellReportMapping holds the report compartment mapping of a cell
 *
 */
struct CellReportMapping
{
    // Global offset within the while simulation frame
    size_t globalOffset;
    // For each section (0...N), relative offset from globalOffset
    std::vector<uint16_t> offsets;
    // For each section (0...N), number of compartments
    std::vector<uint16_t> compartments;
};
