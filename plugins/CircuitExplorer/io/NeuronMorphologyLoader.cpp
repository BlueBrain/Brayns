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

#include "NeuronMorphologyLoader.h"

#include <brayns/utils/Log.h>
#include <brayns/utils/Timer.h>

#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/ModelType.h>
#include <api/coloring/handlers/ComposedColorHandler.h>
#include <api/coloring/methods/MorphologySectionColorMethod.h>
#include <api/coloring/methods/MorphologySectionTypeColorMethod.h>
#include <api/neuron/NeuronMorphologyPipeline.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <api/neuron/builders/NeuronCapsuleBuilder.h>
#include <api/neuron/builders/NeuronSphereBuilder.h>
#include <components/ColorHandler.h>
#include <components/NeuronSectionGeometryMap.h>
#include <components/NeuronSectionType.h>
#include <io/NeuronMorphologyLoaderParameters.h>

namespace
{
class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _components(model.getComponents()),
        _systems(model.getSystems())
    {
    }

    template<typename PrimitiveType>
    void addGeometry(std::vector<PrimitiveType> primitives)
    {
        _components.add<brayns::Geometries>(std::move(primitives));
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setDataSystem<brayns::GeometryDataSystem>();
    }

    void addSections(
        std::vector<SectionTypeMapping> sectionTypeMapping,
        std::vector<SectionSegmentMapping> sectionGeometryMapping)
    {
        _components.add<NeuronSectionType>(std::move(sectionTypeMapping));
        _components.add<NeuronSectionGeometryMap>(std::move(sectionGeometryMapping));
    }

    void addColoring(size_t primitiveCount)
    {
        _components.add<ColorHandler>(std::make_unique<ComposedColorHandler>());

        auto methods = brayns::ColorMethodList();
        methods.push_back(std::make_unique<brayns::SolidColorMethod>());
        methods.push_back(std::make_unique<MorphologySectionColorMethod>(primitiveCount));
        methods.push_back(std::make_unique<MorphologySectionTypeColorMethod>());
        _systems.setColorSystem<brayns::GenericColorSystem>(std::move(methods));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};

class Loader
{
public:
    template<typename PrimitiveType>
    static std::shared_ptr<brayns::Model> load(const std::string &path, const NeuronMorphologyLoaderParameters &input)
    {
        auto morphology = NeuronMorphologyReader::read(path, input.load_soma, input.load_axon, input.load_dendrites);

        auto pipeline = NeuronMorphologyPipeline::fromParameters(input);
        pipeline.process(morphology);

        auto neuronGeometry = NeuronGeometryBuilder<PrimitiveType>::build(morphology);

        auto model = std::make_shared<brayns::Model>(ModelType::morphology);
        auto builder = ModelBuilder(*model);
        builder.addColoring(neuronGeometry.primitives.size());
        builder.addGeometry(std::move(neuronGeometry.primitives));
        builder.addSections(
            std::move(neuronGeometry.sectionTypeMapping),
            std::move(neuronGeometry.sectionSegmentMapping));

        return model;
    }
};

class LoadDispatcher
{
public:
    static std::shared_ptr<brayns::Model> dispatch(
        const std::string &path,
        const NeuronMorphologyLoaderParameters &input)
    {
        auto asSpheres = input.geometry_type == NeuronGeometryType::Spheres;
        if (asSpheres)
        {
            return Loader::load<brayns::Sphere>(path, input);
        }
        return Loader::load<brayns::Capsule>(path, input);
    }
};
} // namespace

std::string NeuronMorphologyLoader::getName() const
{
    return "Neuron Morphology loader";
}

std::vector<std::string> NeuronMorphologyLoader::getExtensions() const
{
    return {"swc", "h5", "asc"};
}

std::vector<std::shared_ptr<brayns::Model>> NeuronMorphologyLoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);
    auto &progress = request.progress;
    auto &params = request.params;

    brayns::Timer timer;

    auto name = getName();

    brayns::Log::info("[CE] {}: loading {}.", name, path);
    progress("Loading " + path, 0.f);

    auto model = LoadDispatcher::dispatch(path, params);

    brayns::Log::info("[CE] {}: done in {} second(s).", name, timer.seconds());

    std::vector<std::shared_ptr<brayns::Model>> result;
    result.push_back(std::move(model));
    return result;
}
