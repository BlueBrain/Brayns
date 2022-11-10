/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "MorphologyCircuitBuilder.h"

#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/ColorSolid.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/ModelType.h>
#include <api/coloring/handlers/ComposedColorHandler.h>
#include <api/coloring/methods/BrainDatasetColorMethod.h>
#include <api/coloring/methods/ElementIdColorMethod.h>
#include <api/coloring/methods/MorphologySectionColorMethod.h>
#include <api/neuron/NeuronGeometryBuilder.h>
#include <api/neuron/NeuronMorphologyPipeline.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <components/BrainColorData.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>
#include <components/NeuronSectionList.h>
#include <systems/NeuronInspectSystem.h>

#include <deque>
#include <future>
#include <unordered_map>

namespace
{
struct MorphologyMap
{
    size_t cellCount{};
    std::unordered_map<std::string, std::vector<size_t>> pathToCellIndices;
};

class MorphologyMapBuilder
{
public:
    static MorphologyMap build(const std::vector<std::string> &paths)
    {
        MorphologyMap result;
        result.cellCount = paths.size();

        auto &mapping = result.pathToCellIndices;

        for (size_t i = 0; i < paths.size(); ++i)
        {
            mapping[paths[i]].push_back(i);
        }

        return result;
    }
};
class ParallelMorphologyLoader
{
public:
    static std::vector<NeuronGeometry> load(
        const MorphologyMap &morphologyMap,
        const NeuronMorphologyLoaderParameters &morphologyParameters,
        const std::vector<brayns::Vector3f> &positions,
        const std::vector<brayns::Quaternion> &rotations,
        ProgressUpdater &progressUpdater)
    {
        const auto cellCount = morphologyMap.cellCount;

        std::vector<NeuronGeometry> morphologies(cellCount);

        const auto soma = morphologyParameters.load_soma;
        const auto axon = morphologyParameters.load_axon;
        const auto dendrites = morphologyParameters.load_dendrites;
        const auto radiusMultiplier = morphologyParameters.radius_multiplier;
        const auto geometryType = morphologyParameters.geometry_type;
        const auto pipeline = NeuronMorphologyPipeline::fromParameters(geometryType, radiusMultiplier);

        const auto loadFn = [&](const std::string &path, const std::vector<size_t> &indices)
        {
            auto morphology = NeuronMorphologyReader::read(path, soma, axon, dendrites);
            pipeline.process(morphology);
            const NeuronGeometryBuilder builder(morphology);
            for (const auto idx : indices)
            {
                morphologies[idx] = builder.instantiate(positions[idx], rotations[idx]);
            }
        };

        const auto updateMessage = std::string("Loading neurons");
        const auto &pathToCellIndexMap = morphologyMap.pathToCellIndices;
        constexpr size_t maxThreads = 800;

        std::deque<std::future<void>> loadTasks;
        for (const auto &[path, cellIndices] : pathToCellIndexMap)
        {
            if (loadTasks.size() == maxThreads)
            {
                auto &topTask = loadTasks.front();
                topTask.get();
                loadTasks.pop_front();
                progressUpdater.update(updateMessage);
            }

            loadTasks.push_back(std::async(loadFn, path, cellIndices));
        }

        for (auto &task : loadTasks)
        {
            task.get();
            progressUpdater.update(updateMessage);
        }

        return morphologies;
    }
};

class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _components(model.getComponents())
        , _systems(model.getSystems())
        , _modelType(model.getType())
    {
    }

    void addIds(std::vector<uint64_t> ids)
    {
        _components.add<CircuitIds>(std::move(ids));
    }

    void addGeometry(std::vector<std::vector<brayns::Capsule>> primitivesList)
    {
        _components.add<brayns::Geometries>(std::move(primitivesList));
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setInitSystem<brayns::GeometryInitSystem>();
        _systems.setCommitSystem<brayns::GeometryCommitSystem>();
        _systems.setInspectSystem<MorphologyInspectSystem>();
    }

    void addNeuronSections(std::vector<std::vector<NeuronSectionMapping>> sections)
    {
        _components.add<NeuronSectionList>(std::move(sections));
    }

    void addColoring(std::unique_ptr<IBrainColorData> data)
    {
        auto availableMethods = data->getMethods();
        auto colorMethods = brayns::ColorMethodList();
        colorMethods.reserve(availableMethods.size() + 3);

        colorMethods.push_back(std::make_unique<brayns::SolidColorMethod>());
        colorMethods.push_back(std::make_unique<ElementIdColorMethod>());
        colorMethods.push_back(std::make_unique<MorphologySectionColorMethod>());
        for (auto method : availableMethods)
        {
            colorMethods.push_back(std::make_unique<BrainDatasetColorMethod>(method));
        }

        _systems.setColorSystem<brayns::GenericColorSystem>(std::move(colorMethods));

        _components.add<ColorHandler>(std::make_unique<ComposedColorHandler>());
        _components.add<BrainColorData>(std::move(data));
    }

    void addDefaultColor()
    {
        if (_modelType == ModelType::neurons)
        {
            _components.add<brayns::ColorSolid>(brayns::Vector4f(1.f, 1.f, 0.f, 1.f));
            return;
        }

        _components.add<brayns::ColorSolid>(brayns::Vector4f(0.55f, 0.7f, 1.f, 1.f));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
    const std::string &_modelType;
};
}

std::vector<CellCompartments>
    MorphologyCircuitBuilder::build(brayns::Model &model, Context context, ProgressUpdater &updater)
{
    auto &morphPaths = context.morphologyPaths;
    auto &ids = context.ids;
    auto &morphParams = context.morphologyParams;
    auto &positions = context.positions;
    auto &rotations = context.rotations;

    auto morphologyPathMap = MorphologyMapBuilder::build(morphPaths);
    auto morphologies = ParallelMorphologyLoader::load(morphologyPathMap, morphParams, positions, rotations, updater);

    std::vector<CellCompartments> compartments(ids.size());
    std::vector<std::vector<brayns::Capsule>> geometries(ids.size());
    std::vector<std::vector<NeuronSectionMapping>> mappings(ids.size());

#pragma omp parallel for
    for (size_t i = 0; i < ids.size(); ++i)
    {
        auto &morphology = morphologies[i];
        auto &sectionMapping = morphology.sectionMapping;
        auto &compartmentMapping = morphology.sectionSegmentMapping;
        auto &geometry = morphology.geometry;

        mappings[i] = std::move(sectionMapping);

        auto &compartment = compartments[i];
        compartment.numItems = geometry.size();
        compartment.sectionSegments = std::move(compartmentMapping);

        geometries[i] = std::move(geometry);
    }

    auto builder = ModelBuilder(model);
    builder.addIds(std::move(ids));
    builder.addGeometry(std::move(geometries));
    builder.addNeuronSections(std::move(mappings));
    builder.addColoring(std::move(context.colorData));
    builder.addDefaultColor();

    return compartments;
}
