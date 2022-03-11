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

#include <bbp/sonata/config.h>

#include <filesystem>
#include <optional>

namespace sonataloader
{
/**
 * @brief The SonataNetworkConfig class holds the parsed circuit & simulation configuration files of a SONATA network
 */
class SonataNetworkConfig
{
public:
    /**
     * @brief Returns the path to the folder where circuit config file is located
     */
    const std::string &circuitConfigDir() const noexcept;

    /**
     * @brief Returns the parsed circuit config file
     */
    const bbp::sonata::CircuitConfig &circuitConfig() const noexcept;

    /**
     * @brief Rreturns the parsed simulation config file
     * @throws std::runtime_error if no simulation configuration file was found while loading
     */
    const bbp::sonata::SimulationConfig &simulationConfig() const;

private:
    friend class SonataConfig;

    SonataNetworkConfig(std::string circuitConfigPath, std::string simulationConfigPath);

    const std::string _circuitConfigDir;
    const bbp::sonata::CircuitConfig _config;
    const std::optional<bbp::sonata::SimulationConfig> _simConfig;
};

class SonataConfig
{
public:
    static SonataNetworkConfig readNetwork(std::string configPath, std::string simConfigPath);

    class MorphologyPath
    {
    public:
        MorphologyPath(const std::string &path, const char *extension);

        std::string buildPath(const std::string &morphologyName) const noexcept;

    private:
        const std::string _path;
        const std::string _extension;
    };

    static MorphologyPath resolveMorphologyPath(const bbp::sonata::PopulationProperties &);

    static std::filesystem::path resolveSpikesPath(const bbp::sonata::SimulationConfig &simConfig);

    static std::filesystem::path resolveReportPath(
        const bbp::sonata::SimulationConfig &simConfig,
        const std::string &report);
};
} // namespace sonataloader
