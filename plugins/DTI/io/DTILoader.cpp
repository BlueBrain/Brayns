/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "DTILoader.h"

#include <brayns/json/JsonObjectMacro.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/FileReader.h>
#include <brayns/utils/Log.h>
#include <brayns/utils/string/StringJoiner.h>

#include <api/ModelType.h>

#include "builder/DTIBuilder.h"
#include "builder/SimulatedDTIBuilder.h"

namespace
{
BRAYNS_JSON_OBJECT_BEGIN(DTIConfiguration)
BRAYNS_JSON_OBJECT_ENTRY(std::string, streamlines_path, "Path to the streamlines file")
BRAYNS_JSON_OBJECT_ENTRY(std::string, gids_to_streamlines_path, "Path to the gid-streamline mapping file")
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    circuit_path,
    "Path to the source circuit for spike simulation",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_END()

class DTIConfigurationReader
{
public:
    static DTIConfiguration read(const std::string &path)
    {
        auto jsonString = brayns::FileReader::read(path);
        auto json = brayns::Json::parse(jsonString);
        auto schema = brayns::Json::getSchema<DTIConfiguration>();
        auto jsonErrors = brayns::JsonSchemaValidator::validate(json, schema);

        if (!jsonErrors.empty())
        {
            auto errorString = brayns::StringJoiner::join(jsonErrors, "\n");
            throw std::invalid_argument("Ill-formed dti config file: " + errorString);
        }

        return brayns::Json::deserialize<DTIConfiguration>(json);
    }
};

class BuilderFactory
{
public:
    static std::unique_ptr<dti::IDTIBuilder> create(const std::string &circuitPath)
    {
        if (!circuitPath.empty())
        {
            return std::make_unique<dti::SimulatedDTIBuilder>();
        }
        return std::make_unique<dti::DTIBuilder>();
    }
};

} // namespace

namespace dti
{
std::string DTILoader::getName() const
{
    return "DTI loader";
}

std::vector<std::string> DTILoader::getSupportedExtensions() const
{
    return {"json"};
}

std::vector<std::unique_ptr<brayns::Model>> DTILoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &callback,
    const DTILoaderParameters &params) const
{
    (void)blob;
    (void)callback;
    (void)params;
    throw std::runtime_error("Loading DTI from blob is not supported");
}

std::vector<std::unique_ptr<brayns::Model>> DTILoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const DTILoaderParameters &params) const
{
    std::vector<std::unique_ptr<brayns::Model>> result;
    result.push_back(std::make_unique<brayns::Model>(ModelType::dti));
    auto &model = *(result.back());

    callback.updateProgress("Reading configuration", 0.f);
    const auto config = DTIConfigurationReader::read(path);

    const auto &circuitPath = config.circuit_path;
    auto builder = BuilderFactory::create(circuitPath);

    callback.updateProgress("Reading gid to row mapping file (might take time)", 0.2f);
    builder->readGidRowFile(config.gids_to_streamlines_path);

    callback.updateProgress("Loading streamlines", 0.4f);
    builder->readStreamlinesFile(config.streamlines_path);

    callback.updateProgress("Generating geometry", 0.6f);
    builder->buildGeometry(params.radius, model);

    callback.updateProgress("Loading simulation", 0.8f);
    builder->buildSimulation(circuitPath, params.spike_decay_time, model);

    callback.updateProgress("Done", 1.f);
    return result;
}
} // namespace dti
