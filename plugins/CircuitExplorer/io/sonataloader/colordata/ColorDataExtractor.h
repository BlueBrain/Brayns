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

#pragma once

#include <bbp/sonata/nodes.h>

#include <string>
#include <vector>

namespace sonataloader
{
/**
 * @brief Quieres a cell node population (biophysical, astrocyte, point_neuron) for the available coloring methods
 *
 */
struct CellNodeColorMethods
{
    static std::vector<std::string> get(const bbp::sonata::NodePopulation &population);
};

/**
 * @brief Quieres a cell node population (biophysical, astrocyte, point_neuron) for the available coloring values
 *
 */
struct CellNodeColorValues
{
    static std::vector<std::string>
        get(const bbp::sonata::NodePopulation &population, const std::string &method, const std::vector<uint64_t> &ids);

    static std::vector<std::string> getAll(const bbp::sonata::NodePopulation &population, const std::string &method);
};
}
