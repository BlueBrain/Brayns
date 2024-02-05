/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>

#include <plugin/io/BBPLoader.h>
#include <plugin/io/bbploader/ParameterCheck.h>
#include <plugin/io/bbploader/simulation/SimulationType.h>

#include <brion/blueConfig.h>

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
    const SonataNGVLoaderParameters&, brayns::Scene&) const
{
    throw std::runtime_error("SonataNGVLoader: Import from blob not supported");
}

std::vector<brayns::ModelDescriptorPtr> SonataNGVLoader::importFromFile(
    const std::string& path, const brayns::LoaderProgress& cb,
    const SonataNGVLoaderParameters& props, brayns::Scene& scene) const
{
    brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    std::vector<brayns::ModelDescriptorPtr> result;

    for (const auto& population : props.populations)
    {
        const auto& name = population.population_name;
        brayns::Log::info("[CE] \t{}: loading population {}.", getName(), name);

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
                                            *config, scene);
        for (auto& model : models)
            model->setName(name + " - " + model->getName());
        result.insert(result.end(), models.begin(), models.end());
    }

    brayns::Log::info("[CE] {}: done in {} second()s.", getName(),
                      timer.elapsed());

    return result;
}
