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

#include <string>
#include <vector>

namespace bbploader
{
/**
 * @brief Queries a circuit file to find out which coloring methods are availables
 *
 */
struct BBPColorMethods
{
    static std::vector<std::string> get(const std::string &circuitPath, const std::string &population);
};

/**
 * @brief Queries a circuit file to get the values of the given method for the given cell ids. If ids is empty,
 * will return all cells values
 *
 */
struct BBPColorValues
{
    static std::vector<std::string> get(
        const std::string &circuitPath,
        const std::string &population,
        const std::string &method,
        const std::vector<uint64_t> &ids);

    static std::vector<std::string>
        getAll(const std::string &circuitPath, const std::string &population, const std::string &method);
};
}
