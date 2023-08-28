/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "CellLoader.h"

#include "color/ColorDataFactory.h"

#include <brayns/engine/components/Metadata.h>
#include <brayns/utils/string/StringJoiner.h>

#include <api/circuit/MorphologyCircuitBuilder.h>
#include <api/circuit/SomaCircuitBuilder.h>

#include <brion/blueConfig.h>

namespace
{
class GlmToRkCommonConverter
{
public:
    template<glm::length_t Size, typename Type>
    static std::vector<brayns::math::vec_t<Type, Size>> convert(const std::vector<glm::vec<Size, Type>> &input)
    {
        auto result = std::vector<brayns::math::vec_t<Type, Size>>();
        result.reserve(input.size());

        for (auto &inputVec : input)
        {
            auto resultVec = result.emplace_back();
            for (glm::length_t i = 0; i < Size; ++i)
            {
                resultVec[i] = inputVec[i];
            }
        }

        return result;
    }

    template<typename Type>
    static std::vector<brayns::math::QuaternionT<Type>> convert(const std::vector<glm::qua<Type>> &input)
    {
        auto result = std::vector<brayns::math::QuaternionT<Type>>();
        result.reserve(input.size());

        for (auto &inputQuat : input)
        {
            result.emplace_back(inputQuat.x, inputQuat.y, inputQuat.z, inputQuat.w);
        }

        return result;
    }
};

class SomaImporter
{
public:
    static std::vector<CellCompartments> import(
        const bbploader::LoadContext &context,
        brayns::Model &model,
        std::unique_ptr<IBrainColorData> colorData)
    {
        auto &circuit = context.circuit;
        auto &gids = context.gids;
        auto ids = std::vector<uint64_t>(gids.begin(), gids.end());

        auto positions = GlmToRkCommonConverter::convert(circuit.getPositions(gids));

        auto &params = context.loadParameters;
        auto &morphParams = params.neuron_morphology_parameters;
        auto radiusMultiplier = morphParams.radius_multiplier;

        auto buildContext =
            SomaCircuitBuilder::Context{std::move(ids), std::move(positions), std::move(colorData), radiusMultiplier};

        return SomaCircuitBuilder::build(model, std::move(buildContext));
    }
};

class MorphologyPathLoader
{
public:
};

class MorphologyImporter
{
public:
    static auto import(
        const bbploader::LoadContext &context,
        brayns::Model &model,
        ProgressUpdater &updater,
        std::unique_ptr<IBrainColorData> colorData)
    {
        auto &circuit = context.circuit;
        auto &gids = context.gids;
        auto ids = std::vector<uint64_t>(gids.begin(), gids.end());
        auto morphPaths = _getMorphologyPaths(circuit, gids);
        auto positions = GlmToRkCommonConverter::convert(circuit.getPositions(gids));
        auto rotations = GlmToRkCommonConverter::convert(circuit.getRotations(gids));
        auto &params = context.loadParameters;
        auto &morphParams = params.neuron_morphology_parameters;

        auto buildContext = MorphologyCircuitBuilder::Context{
            std::move(ids),
            std::move(morphPaths),
            std::move(positions),
            std::move(rotations),
            morphParams,
            std::move(colorData)};

        return MorphologyCircuitBuilder::build(model, std::move(buildContext), updater);
    }

private:
    static std::vector<std::string> _getMorphologyPaths(const brain::Circuit &circuit, const brain::GIDSet &gids)
    {
        auto morphPaths = circuit.getMorphologyURIs(gids);

        auto result = std::vector<std::string>();
        result.reserve(morphPaths.size());

        for (auto &uri : morphPaths)
        {
            result.push_back(uri.getPath());
        }

        return result;
    }
};

class MetadataFactory
{
public:
    static void create(const bbploader::LoadContext &context, brayns::Model &dst)
    {
        auto &metadata = dst.getComponents().add<brayns::Metadata>();

        auto &gids = context.gids;
        auto &params = context.loadParameters;
        auto &targets = params.targets;

        if (targets.has_value())
        {
            auto targetList = brayns::StringJoiner::join(*targets, ",");
            metadata["targets"] = targetList;
        }

        metadata["loaded_neuron_count"] = std::to_string(gids.size());
    }
};
}

namespace bbploader
{
std::vector<CellCompartments> CellLoader::load(
    const LoadContext &context,
    ProgressUpdater &updater,
    brayns::Model &model)
{
    auto &params = context.loadParameters;
    auto &morphSettings = params.neuron_morphology_parameters;
    auto loadSoma = morphSettings.load_soma;
    auto loadAxon = morphSettings.load_axon;
    auto loadDend = morphSettings.load_dendrites;

    MetadataFactory::create(context, model);

    auto colorData = ColorDataFactory::create(context);

    if (loadSoma && !loadAxon && !loadDend)
    {
        return SomaImporter::import(context, model, std::move(colorData));
    }

    return MorphologyImporter::import(context, model, updater, std::move(colorData));
}
} // namespace bbploader
