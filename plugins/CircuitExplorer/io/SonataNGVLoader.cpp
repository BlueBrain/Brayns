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

#include "SonataNGVLoader.h"

#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>

#include <io/BBPLoader.h>
#include <io/bbploader/ParameterCheck.h>
#include <io/bbploader/simulation/SimulationType.h>

#include <brion/blueConfig.h>

namespace
{
struct BlueConfigReader
{
    static brion::BlueConfig read(const std::string &populationName, const std::string &path)
    {
        if (populationName == "Default")
        {
            return brion::BlueConfig(path);
        }

        return brion::BlueConfig(path, brion::BlueConfigSection::CONFIGSECTION_CIRCUIT, populationName);
    }
};
}

std::vector<std::string> SonataNGVLoader::getSupportedExtensions() const
{
    return _internal.getSupportedExtensions();
}

bool SonataNGVLoader::isSupported(const std::string &filename, const std::string &extension) const
{
    return _internal.isSupported(filename, extension);
}

std::string SonataNGVLoader::getName() const
{
    return std::string("SONATA NGV loader");
}

std::vector<std::unique_ptr<brayns::Model>> SonataNGVLoader::importFromBlob(
    brayns::Blob &&blob,
    const brayns::LoaderProgress &cb,
    const SonataNGVLoaderParameters &params) const
{
    (void) blob;
    (void) cb;
    (void) params;
    throw std::runtime_error("SonataNGVLoader: Import from blob not supported");
}

std::vector<std::unique_ptr<brayns::Model>> SonataNGVLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &cb,
    const SonataNGVLoaderParameters &props) const
{
    brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    std::vector<std::unique_ptr<brayns::Model>> result;

    for (const auto &population : props.populations)
    {
        const auto &name = population.population_name;
        brayns::Log::info("[CE] \t{}: loading population {}.", getName(), name);

        const brion::BlueConfig config = BlueConfigReader::read(name, path);
        auto models = _internal.importFromBlueConfig(cb, population.circuit_config, config);
        for(auto &model : models)
        {
            result.push_back(std::move(model));
        }
    }

    brayns::Log::info("[CE] {}: done in {} second(s).", getName(), timer.seconds());

    return result;
}
