/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/ModelType.h>
#include <api/coloring/handlers/ComposedColorHandler.h>
#include <api/coloring/methods/BrainDatasetColorMethod.h>
#include <api/coloring/methods/IdColorMethod.h>
#include <api/coloring/methods/MorphologySectionTypeColorMethod.h>
#include <api/neuron/NeuronMorphologyPipeline.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <api/neuron/builders/NeuronCapsuleBuilder.h>
#include <api/neuron/builders/NeuronSphereBuilder.h>
#include <components/BrainColorData.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>
#include <components/NeuronSectionType.h>
#include <systems/NeuronInspectSystem.h>

#include <deque>
#include <future>
#include <unordered_map>

namespace
{
/**
 * @brief builds a map of morphology file paths to all the morphology indices that uses such file.
 */
class MorphologyPathMap
{
public:
    static std::unordered_map<std::string, std::vector<size_t>> build(const std::vector<std::string> &paths)
    {
        auto result = std::unordered_map<std::string, std::vector<size_t>>();
        for (size_t i = 0; i < paths.size(); ++i)
        {
            result[paths[i]].push_back(i);
        }
        return result;
    }
};

/**
 * @brief Reads the morphology files and transform them into geometry.
 */
class ParallelMorphologyLoader
{
public:
    static inline constexpr size_t maxThreads = 800;

    template<typename PrimitiveType>
    static std::vector<NeuronGeometry<PrimitiveType>> load(
        const std::unordered_map<std::string, std::vector<size_t>> &morphologyMap,
        const NeuronMorphologyLoaderParameters &morphologyParameters,
        const std::vector<brayns::Vector3f> &positions,
        const std::vector<brayns::Quaternion> &rotations,
        ProgressUpdater &progressUpdater)
    {
        auto morphologies = std::vector<NeuronGeometry<PrimitiveType>>(positions.size());

        auto soma = morphologyParameters.load_soma;
        auto axon = morphologyParameters.load_axon;
        auto dendrites = morphologyParameters.load_dendrites;
        auto pipeline = NeuronMorphologyPipeline::fromParameters(morphologyParameters);

        auto loadFn = [&](const std::string &path, const std::vector<size_t> &indices)
        {
            auto morphology = NeuronMorphologyReader::read(path, soma, axon, dendrites);
            pipeline.process(morphology);

            auto baseGeometry = NeuronGeometryBuilder<PrimitiveType>::build(morphology);

            for (auto idx : indices)
            {
                morphologies[idx] =
                    NeuronGeometryInstantiator<PrimitiveType>::instantiate(baseGeometry, positions[idx], rotations[idx]);
            }
        };

        auto updateMessage = std::string("Loading neurons");
        auto loadTasks = std::deque<std::future<void>>();

        for (auto &[path, cellIndices] : morphologyMap)
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

/**
 * Builder pattern with all needed steps to create a model with a neuron/astrocyte circuit on it.
 */
class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _components(model.getComponents()),
        _systems(model.getSystems()),
        _modelType(model.getType())
    {
    }

    void addIds(std::vector<uint64_t> ids)
    {
        _components.add<CircuitIds>(std::move(ids));
    }

    template<typename PrimitiveType>
    void addGeometry(std::vector<std::vector<PrimitiveType>> primitivesList)
    {
        _components.add<brayns::Geometries>(std::move(primitivesList));
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setDataSystem<brayns::GeometryDataSystem>();
        _systems.setInspectSystem<MorphologyInspectSystem>();
    }

    void addNeuronSections(std::vector<std::vector<SectionTypeMapping>> sections)
    {
        _components.add<NeuronSectionType>(std::move(sections));
    }

    void addColoring(std::unique_ptr<IBrainColorData> data)
    {
        auto availableMethods = data->getMethods();
        auto colorMethods = brayns::ColorMethodList();
        colorMethods.reserve(availableMethods.size() + 3);

        colorMethods.push_back(std::make_unique<brayns::SolidColorMethod>());
        colorMethods.push_back(std::make_unique<IdColorMethod>());
        colorMethods.push_back(std::make_unique<MorphologySectionTypeColorMethod>());
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
        // Neurons are yellow
        if (_modelType == ModelType::neurons)
        {
            _components.add<brayns::ColorSolid>(brayns::Vector4f(1.f, 1.f, 0.f, 1.f));
            return;
        }

        // Astrocytes are blue
        _components.add<brayns::ColorSolid>(brayns::Vector4f(0.55f, 0.7f, 1.f, 1.f));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
    const std::string &_modelType;
};

/**
 * @brief Flattens the morphology data into separate containers.
 */
template<typename PrimitiveType>
class DataFlattener
{
public:
    struct FlatData
    {
        std::vector<std::vector<SectionTypeMapping>> sectionTypeMappings;
        std::vector<CellCompartments> compartments;
        std::vector<std::vector<PrimitiveType>> geometries;

        FlatData(std::size_t size)
        {
            sectionTypeMappings.reserve(size);
            compartments.reserve(size);
            geometries.reserve(size);
        }

        void flattenElement(NeuronGeometry<PrimitiveType> &element)
        {
            sectionTypeMappings.push_back(std::move(element.sectionTypeMapping));
            compartments.push_back({element.primitives.size(), std::move(element.sectionSegmentMapping)});
            geometries.push_back(std::move(element.primitives));
        }
    };

    static FlatData flatten(std::vector<NeuronGeometry<PrimitiveType>> &input)
    {
        auto flatData = FlatData(input.size());
        for (auto &element : input)
        {
            flatData.flattenElement(element);
        }
        return flatData;
    }
};

template<typename PrimitiveType>
class Builder
{
public:
    static std::vector<CellCompartments> build(
        brayns::Model &model,
        MorphologyCircuitBuilder::Context context,
        ProgressUpdater &updater)
    {
        auto morphologies = ParallelMorphologyLoader::load<PrimitiveType>(
            MorphologyPathMap::build(context.morphologyPaths),
            context.morphologyParams,
            context.positions,
            context.rotations,
            updater);

        auto data = DataFlattener<PrimitiveType>::flatten(morphologies);

        auto builder = ModelBuilder(model);
        builder.addIds(std::move(context.ids));
        builder.addGeometry(std::move(data.geometries));
        builder.addNeuronSections(std::move(data.sectionTypeMappings));
        builder.addColoring(std::move(context.colorData));
        builder.addDefaultColor();

        return data.compartments;
    }
};

class BuildDispatcher
{
public:
    static std::vector<CellCompartments> dispatch(
        brayns::Model &model,
        MorphologyCircuitBuilder::Context context,
        ProgressUpdater &updater)
    {
        auto geometryType = context.morphologyParams.geometry_type;
        if (geometryType == NeuronGeometryType::Spheres)
        {
            return Builder<brayns::Sphere>::build(model, std::move(context), updater);
        }
        return Builder<brayns::Capsule>::build(model, std::move(context), updater);
    }
};
}

std::vector<CellCompartments> MorphologyCircuitBuilder::build(
    brayns::Model &model,
    Context context,
    ProgressUpdater &updater)
{
    return BuildDispatcher::dispatch(model, std::move(context), updater);
}
