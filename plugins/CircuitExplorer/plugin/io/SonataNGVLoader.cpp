/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "SonataNGVLoader.h"

#include <brayns/common/Timer.h>

#include <plugin/api/Log.h>
#include <plugin/io/BBPLoader.h>
#include <plugin/io/bbploader/ParameterCheck.h>
#include <plugin/io/bbploader/simulation/SimulationType.h>

#include <brion/blueConfig.h>

SonataNGVLoader::SonataNGVLoader(brayns::Scene& scene)
    : brayns::Loader<SonataNGVLoaderParameters>(scene)
    , _internal(std::make_unique<BBPLoader>(scene))
{
}

std::vector<std::string> SonataNGVLoader::getSupportedExtensions() const
{
    return _internal->getSupportedExtensions();
}

bool SonataNGVLoader::isSupported(const std::string& filename,
                                  const std::string& extension) const
{
    return _internal->isSupported(filename, extension);
}

std::string SonataNGVLoader::getName() const
{
    return std::string("SONATA NGV loader");
}

std::vector<brayns::ModelDescriptorPtr> SonataNGVLoader::importFromBlob(
    brayns::Blob&&, const brayns::LoaderProgress&,
    const SonataNGVLoaderParameters&) const
{
    throw std::runtime_error("SonataNGVLoader: Import from blob not supported");
}

std::vector<brayns::ModelDescriptorPtr> SonataNGVLoader::importFromFile(
    const std::string& path, const brayns::LoaderProgress& cb,
    const SonataNGVLoaderParameters& props) const
{
    brayns::Timer timer;
    PLUGIN_INFO << getName() << ": Loading " << path << std::endl;

    std::vector<brayns::ModelDescriptorPtr> result;

    for (const auto& population : props.populations)
    {
        const auto& name = population.population_name;
        PLUGIN_INFO << "\t" << getName() << ": Loading population " << name
                    << std::endl;

        // Load the BlueConfig/CircuitConfig
        std::unique_ptr<brion::BlueConfig> config;
        if (name == "Default")
            config = std::make_unique<brion::BlueConfig>(path);
        else
            config = std::make_unique<brion::BlueConfig>(
                path, brion::BlueConfigSection::CONFIGSECTION_CIRCUIT, name);

        // Import the model
        auto models =
            _internal->importFromBlueConfig(path, cb, population.circuit_config,
                                            *config);
        for (auto& model : models)
            model->setName(name + " - " + model->getName());
        result.insert(result.end(), models.begin(), models.end());
    }

    PLUGIN_INFO << getName() << ": Done in " << timer.elapsed() << " second(s)"
                << std::endl;
    return result;
}
