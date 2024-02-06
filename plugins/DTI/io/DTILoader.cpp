/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/json/Json.h>
#include <brayns/utils/FileReader.h>
#include <brayns/utils/Log.h>
#include <brayns/utils/string/StringJoiner.h>

#include <api/ModelType.h>

#include "builder/DTIBuilder.h"
#include "builder/SimulatedDTIBuilder.h"

namespace
{
struct DTIConfiguration
{
    std::string streamlines_path;
    std::string gids_to_streamlines_path;
    std::string circuit_path;
};
} // namespace

namespace brayns
{
template<>
struct JsonAdapter<DTIConfiguration> : ObjectAdapter<DTIConfiguration>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("DTIConfiguration");
        builder
            .getset(
                "streamlines_path",
                [](auto &object) -> auto & { return object.streamlines_path; },
                [](auto &object, auto value) { object.streamlines_path = std::move(value); })
            .description("Path to the streamlines file");
        builder
            .getset(
                "gids_to_streamlines_path",
                [](auto &object) -> auto & { return object.gids_to_streamlines_path; },
                [](auto &object, auto value) { object.gids_to_streamlines_path = std::move(value); })
            .description("Path to the gid-streamline mapping file");
        builder
            .getset(
                "circuit_path",
                [](auto &object) -> auto & { return object.circuit_path; },
                [](auto &object, auto value) { object.circuit_path = std::move(value); })
            .description("Path to the source circuit for spike simulation")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns

namespace
{
class DTIConfigurationReader
{
public:
    static DTIConfiguration read(const std::string &path)
    {
        auto data = brayns::FileReader::read(path);
        auto json = brayns::Json::parse(data);
        auto schema = brayns::Json::getSchema<DTIConfiguration>();
        auto errors = brayns::Json::validate(json, schema);
        if (!errors.empty())
        {
            throw brayns::JsonSchemaException("Invalid DTI configuration", errors);
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

std::vector<std::string> DTILoader::getExtensions() const
{
    return {"json"};
}

std::vector<std::shared_ptr<brayns::Model>> DTILoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);
    auto &progress = request.progress;
    auto &params = request.params;

    std::vector<std::shared_ptr<brayns::Model>> result;
    result.push_back(std::make_shared<brayns::Model>(ModelType::dti));
    auto &model = *(result.back());

    progress("Reading configuration", 0.f);
    const auto config = DTIConfigurationReader::read(path);

    const auto &circuitPath = config.circuit_path;
    auto builder = BuilderFactory::create(circuitPath);

    progress("Reading gid to row mapping file (might take time)", 0.2f);
    builder->readGidRowFile(config.gids_to_streamlines_path);

    progress("Loading streamlines", 0.4f);
    builder->readStreamlinesFile(config.streamlines_path);

    progress("Generating geometry", 0.6f);
    builder->buildGeometry(params.radius, model);

    progress("Loading simulation", 0.8f);
    builder->buildSimulation(circuitPath, params.spike_decay_time, model);

    progress("Done", 1.f);
    return result;
}
} // namespace dti
