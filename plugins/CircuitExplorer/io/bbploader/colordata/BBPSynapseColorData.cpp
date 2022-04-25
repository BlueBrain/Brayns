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

#include "BBPSynapseColorData.h"

#include <io/bbploader/colordata/ColorDataExtractor.h>

#include <set>

namespace bbploader
{
BBPSynapseColorData::BBPSynapseColorData(std::string circuitPath, std::string circuitPopulation)
    : _circuitPath(std::move(circuitPath))
    , _circuitPop(std::move(circuitPopulation))
{
}

std::vector<std::string> BBPSynapseColorData::getMethods() const noexcept
{
    return BBPColorMethods::get(_circuitPath, _circuitPop);
}

std::vector<std::string> BBPSynapseColorData::getMethodVariables(const std::string &method) const
{
    const auto variables = BBPColorValues::getAll(_circuitPath, _circuitPop, method);
    const std::set<std::string> unique(variables.begin(), variables.end());
    return std::vector<std::string>(unique.begin(), unique.end());
}

std::vector<std::string> BBPSynapseColorData::getMethodValuesForIDs(
    const std::string &method,
    const std::vector<uint64_t> &ids) const
{
    return BBPColorValues::get(_circuitPath, _circuitPop, method, ids);
}
} // namespace bbploader
