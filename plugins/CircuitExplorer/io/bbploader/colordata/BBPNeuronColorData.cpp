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

#include "BBPNeuronColorData.h"

#include <api/neuron/NeuronColorMethods.h>
#include <api/neuron/NeuronSection.h>
#include <io/bbploader/colordata/ColorDataExtractor.h>

#include <set>

namespace bbploader
{
BBPNeuronColorData::BBPNeuronColorData(std::string circuitPath, std::string circuitPop)
    : _circuitPath(std::move(circuitPath))
    , _circuitPop(std::move(circuitPop))
{
}

std::vector<std::string> BBPNeuronColorData::getMethods() const noexcept
{
    auto circuitMethods = BBPColorMethods::get(_circuitPath, _circuitPop);

    constexpr auto morphologySectionMethod = NeuronColorMethods::BY_MORPHOLOGY_SECTION;
    const auto methodName = brayns::enumToString(morphologySectionMethod);
    circuitMethods.push_back(std::move(methodName));

    return circuitMethods;
}

std::vector<std::string> BBPNeuronColorData::getMethodVariables(const std::string &method) const
{
    const auto methodEnum = brayns::stringToEnum<NeuronColorMethods>(method);
    if (methodEnum == NeuronColorMethods::BY_MORPHOLOGY_SECTION)
    {
        return brayns::enumNames<NeuronSection>();
    }

    const auto variables = BBPColorValues::getAll(_circuitPath, _circuitPop, method);
    const std::set<std::string> unique(variables.begin(), variables.end());
    return std::vector<std::string>(unique.begin(), unique.end());
}

std::vector<std::string> BBPNeuronColorData::getMethodValuesForIDs(
    const std::string &method,
    const std::vector<uint64_t> &ids) const
{
    return BBPColorValues::get(_circuitPath, _circuitPop, method, ids);
}
} // namespace bbploader
