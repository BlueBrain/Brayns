/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <string>
#include <vector>

#include "BrainColorMethod.h"

/**
 * @brief Interface to implement coloring method information fetchers for different neuronal circuit formats.
 */
class IBrainColorData
{
public:
    virtual ~IBrainColorData() = default;

    /**
     * @brief Return the available color methods supported by the color data implementing it.
     * @return std::vector<BrainColorMethod>
     */
    virtual std::vector<BrainColorMethod> getMethods() const = 0;

    /**
     * @brief Return the input values corresponding to the given ids for the given method.
     * @param method Method from which to extract values.
     * @param ids Ids used to fetch the data.
     * @return std::vector<std::string>
     * @throws std::invalid_argument if the method is not supported.
     */
    virtual std::vector<std::string> getValues(BrainColorMethod method, const std::vector<uint64_t> &ids) const = 0;
};
