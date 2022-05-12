/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/parameters/NetworkParameters.h>
#include <brayns/common/parameters/SimulationParameters.h>

namespace brayns
{
/**
 * @brief Class managing all parameters registered by the application. Additional parameter objects may be registered.
 */
class ParametersManager
{
public:
    ParametersManager(int argc, const char **argv);

    /**
     * @brief Registers specific parameters to the manager
     * @param parameters to be registered
     */
    void registerParameters(AbstractParameters *parameters);

    /**
     * @brief Displays usage of registered parameters
     */
    void usage();

    /**
     * @brief Displays values registered parameters
     */
    void print();

    /**
     * @brief Gets simulation parameters
     * @return Simulation parameters for the current scene
     */
    SimulationParameters &getSimulationParameters();
    const SimulationParameters &getSimulationParameters() const;

    /**
     * @brief Gets application parameters
     * @return Application parameters for the current scene
     */
    ApplicationParameters &getApplicationParameters();
    const ApplicationParameters &getApplicationParameters() const;

    /**
     * @brief Gets volume parameters
     * @return Parameters for the current volume
     */
    NetworkParameters &getNetworkParameters();
    const NetworkParameters &getNetworkParameters() const;

    /**
     * @brief resetModified resets the modified status for all registered parameters
     */
    void resetModified();

private:
    void _parse(int argc, const char **argv);
    void _processUnrecognizedOptions(const std::vector<std::string> &unrecognizedOptions) const;

    po::options_description _allOptions;

    std::vector<AbstractParameters *> _parameterSets;
    SimulationParameters _simulationParameters;
    ApplicationParameters _applicationParameters;
    NetworkParameters _networkParameters;
};
} // namespace brayns
