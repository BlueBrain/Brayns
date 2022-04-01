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

namespace bbploader
{
BBPSynapseColorData::BBPSynapseColorData(std::string circuitPath, std::string circuitPopulation)
 : _internal(std::move(circuitPath), std::move(circuitPopulation))
{
}

std::vector<std::string> BBPSynapseColorData::getMethods() const noexcept
{
    return _internal.getCircuitLevelMethods();
}

std::vector<std::string> BBPSynapseColorData::getMethodVariables(const std::string &method) const
{
    return _internal.getCircuitLevelMethodVariables(method);
}

std::vector<std::string> BBPSynapseColorData::getMethodValuesForIDs(
        const std::string &method, const std::vector<uint64_t>& ids) const
{
    return _internal.getMethodValuesForIDs(method, ids);
}
} // namespace bbploader
