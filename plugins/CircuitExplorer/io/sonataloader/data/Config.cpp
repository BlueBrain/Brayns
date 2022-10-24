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

#include "Config.h"

namespace
{
class PathResolver
{
public:
    static std::string resolveParentPath(const std::string &configPath)
    {
        std::filesystem::path base(configPath);
        base = base.parent_path();
        base = base.lexically_normal();
        return base.string();
    }
};

struct MorphologyFormat
{
    inline static const std::string swc = "swc";
    inline static const std::string h5 = "h5v1";
    inline static const std::string ascii = "neurolucida-asc";
};

class MorphologyPathResolver
{
public:
    static std::string getFormatPath(const bbp::sonata::PopulationProperties &params, const std::string &format)
    {
        if (format == MorphologyFormat::swc)
        {
            return params.morphologiesDir;
        }

        auto &alternateFormats = params.alternateMorphologyFormats;
        auto it = alternateFormats.find(format);
        if (it == alternateFormats.end())
        {
            return {};
        }

        return it->second;
    }

    static bool isValid(const std::string &path, std::string_view extension)
    {
        auto pathObject = std::filesystem::path(path);
        if (!std::filesystem::is_directory(pathObject))
        {
            return false;
        }

        auto testExtension = std::string(".") + std::string(extension);
        for (auto &item : std::filesystem::directory_iterator(pathObject))
        {
            if (!std::filesystem::is_regular_file(item))
            {
                continue;
            }

            auto &file = item.path();
            auto fileExtension = file.extension().string();
            if (fileExtension == testExtension)
            {
                return true;
            }
        }
        return false;
    }
};

class SimulationFactory
{
public:
    static std::optional<bbp::sonata::SimulationConfig> tryToCreate(const std::string &path)
    {
        try
        {
            return std::make_optional(bbp::sonata::SimulationConfig::fromFile(path));
        }
        catch (const bbp::sonata::SonataError &)
        {
        }

        return {};
    }
};

class ReportPathResolver
{
public:
    static std::filesystem::path resolveBasePath(const bbp::sonata::SimulationConfig &config)
    {
        auto &output = config.getOutput();
        auto basePath = std::filesystem::path(output.outputDir);
        if (!basePath.is_absolute())
        {
            basePath = std::filesystem::path(config.getBasePath()) / basePath;
        }

        return basePath;
    }
};
} // namespace

namespace sonataloader
{
MorphologyPath::MorphologyPath(std::string path, std::string extension)
    : _path(std::move(path))
    , _extension(std::move(extension))
{
}

std::string MorphologyPath::buildPath(const std::string &morphologyName) const noexcept
{
    return _path + "/" + morphologyName + "." + _extension;
}

Config::Config(const std::string &path)
    : _circuitConfigDir(PathResolver::resolveParentPath(path))
    , _simConfig(SimulationFactory::tryToCreate(path))
    , _config(bbp::sonata::CircuitConfig::fromFile(_simConfig ? _simConfig->getNetwork() : path))
{
}

const std::string &Config::getBasePath() const noexcept
{
    return _circuitConfigDir;
}

const std::string &Config::getConfigAsJson() const noexcept
{
    return _config.getExpandedJSON();
}

const std::string &Config::getNodesetsPath() const
{
    return _config.getNodeSetsPath();
}

bbp::sonata::NodePopulation Config::getNodes(const std::string &name) const
{
    return _config.getNodePopulation(name);
}

bbp::sonata::PopulationProperties Config::getNodesProperties(const std::string &name) const
{
    return _config.getNodePopulationProperties(name);
}

std::set<std::string> Config::getAllNodeNames() const noexcept
{
    return _config.listNodePopulations();
}

bbp::sonata::EdgePopulation Config::getEdges(const std::string &name) const
{
    return _config.getEdgePopulation(name);
}

bbp::sonata::PopulationProperties Config::getEdgesProperties(const std::string &name) const
{
    return _config.getEdgePopulationProperties(name);
}

std::set<std::string> Config::getAllEdgeNames() const noexcept
{
    return _config.listEdgePopulations();
}

MorphologyPath Config::getMorphologyPath(const std::string &populationName) const
{
    auto properties = _config.getNodePopulationProperties(populationName);

    if (MorphologyPathResolver::isValid(properties.morphologiesDir, "swc"))
    {
        return MorphologyPath(properties.morphologiesDir, MorphologyFormat::swc);
    }

    auto asciiPath = MorphologyPathResolver::getFormatPath(properties, MorphologyFormat::ascii);
    if (MorphologyPathResolver::isValid(asciiPath, "asc"))
    {
        return MorphologyPath(asciiPath, "asc");
    }

    auto h5Path = MorphologyPathResolver::getFormatPath(properties, MorphologyFormat::h5);
    if (MorphologyPathResolver::isValid(h5Path, "h5"))
    {
        return MorphologyPath(h5Path, "h5");
    }

    throw std::runtime_error("SonataConfig: Morphology path not available");
}

std::string Config::getSpikesPath() const
{
    if (!_simConfig)
    {
        throw std::runtime_error("No simulation configuration available");
    }

    auto basePath = ReportPathResolver::resolveBasePath(*_simConfig);
    auto &output = _simConfig->getOutput();
    return (basePath / std::filesystem::path(output.spikesFile)).lexically_normal().string();
}

std::string Config::getReportPath(const std::string &reportName) const
{
    if (!_simConfig)
    {
        throw std::runtime_error("No simulation configuration available");
    }

    auto reports = _simConfig->listReportNames();
    if (reports.find(reportName) == reports.end())
    {
        throw std::invalid_argument("The report does not exists in the network");
    }

    auto basePath = ReportPathResolver::resolveBasePath(*_simConfig);
    return (basePath / std::filesystem::path(reportName + ".h5")).lexically_normal().string();
}
} // namespace sonataloader
