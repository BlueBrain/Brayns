/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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
#include <bbp/sonata/node_sets.h>

#include <filesystem>
#include <optional>

namespace sonataloader
{
class MorphologyPath
{
public:
    MorphologyPath(std::string path, std::string extension);

    std::string buildPath(const std::string &morphologyName) const noexcept;

private:
    std::string _path;
    std::string _extension;
};

/**
 * @brief Facade to interact with SONATA circuit configuration & simulation configuration
 */
class Config
{
public:
    explicit Config(const std::string &path);

    /**
     * @brief Returns the configuration file parent path.
     * @return const std::string&
     */
    const std::string &getBasePath() const noexcept;

    /**
     * @brief Returns the configuration file as a JSON string, with the paths expanded with the manifest
     * information
     * @return const std::string&
     */
    const std::string &getConfigAsJson() const noexcept;

    /**
     * @brief Extract node sets if any.
     * @return std::optional<bbp::sonata::NodeSets>
     */
    std::optional<bbp::sonata::NodeSets> getNodeSets() const;

    /**
     * @brief Returns the population object of the given population name.
     * @param name Name of the node population.
     * @return bbp::sonata::NodePopulation
     */
    bbp::sonata::NodePopulation getNodes(const std::string &name) const;

    /**
     * @brief Returns the node population properties of the given population name.
     * @param name Name of the node population.
     * @return bbp::sonata::PopulationProperties
     */
    bbp::sonata::NodePopulationProperties getNodesProperties(const std::string &name) const;

    /**
     * @brief Return the name of all node populations in the network.
     * @return std::set<std::string>
     */
    std::set<std::string> getAllNodeNames() const noexcept;

    /**
     * @brief Return the population object of the given population name.
     * @param name Name of the edge population.
     * @return bbp::sonata::EdgePopulation
     */
    bbp::sonata::EdgePopulation getEdges(const std::string &name) const;

    /**
     * @brief Returns the edge population properties of the given population name.
     * @param name Name of the edge population.
     * @return bbp::sonata::PopulationProperties
     */
    bbp::sonata::EdgePopulationProperties getEdgesProperties(const std::string &name) const;

    /**
     * @brief Returns any available path to the directory containing the population morphologies.
     * @param populationName Name of the node population.
     * @return MorphologyPath the morphology path resolver object.
     * @throws std::runtime_error if not available path exists for the given population.
     */
    MorphologyPath getMorphologyPath(const std::string &populationName) const;

    /**
     * @brief Returns the path to the spike report file.
     * @return std::string
     * @throws std::runtime_error if no simulation config is available.
     */
    std::string getSpikesPath() const;

    /**
     * @brief Returns the path to the report indicated by the given reportName.
     * @param reportName Name of the report.
     * @return std::string
     * @throws std::runtime_error if no simulation config is available.
     * @throws std::invalid_argument if the report does not exists in the network.
     */
    std::string getReportPath(const std::string &reportName) const;

private:
    std::string _circuitConfigDir;
    std::optional<bbp::sonata::SimulationConfig> _simConfig;
    bbp::sonata::CircuitConfig _config;
};
} // namespace sonataloader
