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

#include "SonataConfig.h"

namespace
{
constexpr char H5_FORMAT[] = "h5v1";
constexpr char ASCII_FORMAT[] = "neurolucida-asc";

std::string getParentDirPath(const std::string &circuitConfigPath)
{
    std::filesystem::path base(circuitConfigPath);
    base = base.parent_path();
    base = base.lexically_normal();
    return base.string();
}

std::optional<bbp::sonata::SimulationConfig> getSimulationConfig(
    const std::string &circuitConfigDirPath,
    const std::string &simulationConfigPath)
{
    std::filesystem::path simConfigPath(simulationConfigPath);
    std::filesystem::path configDirPath(circuitConfigDirPath);

    if (std::filesystem::exists(simConfigPath))
    {
        return std::optional<bbp::sonata::SimulationConfig>(
            bbp::sonata::SimulationConfig::fromFile(simulationConfigPath));
    }
    else
    {
        std::filesystem::path configDirPath(circuitConfigDirPath);
        if (simConfigPath.is_relative() && std::filesystem::is_regular_file(configDirPath / simConfigPath))
        {
            const auto composedPath = (configDirPath / simConfigPath).string();
            return bbp::sonata::SimulationConfig::fromFile(composedPath);
        }
        else if (std::filesystem::exists(configDirPath / std::filesystem::path("simulation_sonata.json")))
        {
            const auto composedPath = (configDirPath / std::filesystem::path("simulation_sonata.json")).string();
            return std::optional<bbp::sonata::SimulationConfig>(bbp::sonata::SimulationConfig::fromFile(composedPath));
        }
    }

    return std::nullopt;
}

std::filesystem::path resolveSimulationBasePath(const bbp::sonata::SimulationConfig &config)
{
    const auto &output = config.getOutput();

    std::filesystem::path basePath(output.outputDir);
    if (!basePath.is_absolute())
    {
        const auto &configBasePath = config.getBasePath();
        basePath = std::filesystem::path(configBasePath) / basePath;
    }

    return basePath;
}
} // namespace

namespace sonataloader
{
const std::string &SonataNetworkConfig::circuitConfigDir() const noexcept
{
    return _circuitConfigDir;
}

const bbp::sonata::CircuitConfig &SonataNetworkConfig::circuitConfig() const noexcept
{
    return _config;
}

const bbp::sonata::SimulationConfig &SonataNetworkConfig::simulationConfig() const
{
    if (!_simConfig.has_value())
    {
        throw std::runtime_error("Could not find a simulation configuration file");
    }

    return _simConfig.value();
}

SonataNetworkConfig::SonataNetworkConfig(std::string circuitConfigPath, std::string simulationConfigPath)
    : _circuitConfigDir(getParentDirPath(circuitConfigPath))
    , _config(bbp::sonata::CircuitConfig::fromFile(circuitConfigPath))
    , _simConfig(getSimulationConfig(_circuitConfigDir, simulationConfigPath))
{
}

SonataNetworkConfig SonataConfig::readNetwork(std::string configPath, std::string simConfigPath)
{
    return SonataNetworkConfig(configPath, simConfigPath);
}

SonataConfig::MorphologyPath::MorphologyPath(const std::string &path, const char *extension)
    : _path(path)
    , _extension(extension)
{
}

std::string SonataConfig::MorphologyPath::buildPath(const std::string &morphologyName) const noexcept
{
    return _path + "/" + morphologyName + "." + _extension;
}

SonataConfig::MorphologyPath SonataConfig::resolveMorphologyPath(const bbp::sonata::PopulationProperties &properties)
{
    if (std::filesystem::exists(properties.morphologiesDir))
        return SonataConfig::MorphologyPath(properties.morphologiesDir, "swc");

    const auto h5It = properties.alternateMorphologyFormats.find(H5_FORMAT);
    if (h5It != properties.alternateMorphologyFormats.end() && std::filesystem::exists(h5It->second))
        return SonataConfig::MorphologyPath(h5It->second, "h5");

    const auto ascIt = properties.alternateMorphologyFormats.find(ASCII_FORMAT);
    if (ascIt != properties.alternateMorphologyFormats.end() && std::filesystem::exists(ascIt->second))
        return SonataConfig::MorphologyPath(ascIt->second, "asc");

    throw std::runtime_error("SonataConfig: Could not determine morphology path");
}

std::filesystem::path SonataConfig::resolveSpikesPath(const bbp::sonata::SimulationConfig &config)
{
    auto basePath = resolveSimulationBasePath(config);

    const auto &output = config.getOutput();

    basePath /= std::filesystem::path(output.spikesFile);

    return basePath.lexically_normal();
}

std::filesystem::path SonataConfig::resolveReportPath(
    const bbp::sonata::SimulationConfig &config,
    const std::string &report)
{
    if (report.empty())
    {
        throw std::invalid_argument("Cannot resolve a report path with an empty report name");
    }

    auto basePath = resolveSimulationBasePath(config);

    basePath /= std::filesystem::path(report + ".h5");

    return basePath.lexically_normal();
}
} // namespace sonataloader
