/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/core/parameters/ParametersManager.h>

#include "argv/ArgvParser.h"

namespace brayns
{
ParametersManager::ParametersManager(int argc, const char **argv)
{
    if (argc <= 1)
    {
        return;
    }
    auto properties = getArgvProperties();
    ArgvParser parser(properties);
    parser.parse(argc, argv);
}

std::vector<ArgvProperty> ParametersManager::getArgvProperties()
{
    std::vector<brayns::ArgvProperty> properties;
    brayns::ArgvBuilder builder(properties);
    forEach([&](auto &parameters) { parameters.build(builder); });
    return properties;
}

SimulationParameters &ParametersManager::getSimulationParameters()
{
    return _simulationParameters;
}

const SimulationParameters &ParametersManager::getSimulationParameters() const
{
    return _simulationParameters;
}

ApplicationParameters &ParametersManager::getApplicationParameters()
{
    return _applicationParameters;
}

const ApplicationParameters &ParametersManager::getApplicationParameters() const
{
    return _applicationParameters;
}

NetworkParameters &ParametersManager::getNetworkParameters()
{
    return _networkParameters;
}

const NetworkParameters &ParametersManager::getNetworkParameters() const
{
    return _networkParameters;
}

void ParametersManager::resetModified()
{
    forEach([](auto &parameters) { parameters.resetModified(); });
}
} // namespace brayns
