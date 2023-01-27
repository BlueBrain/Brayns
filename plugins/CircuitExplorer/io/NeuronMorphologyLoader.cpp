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
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/ModelType.h>
#include <api/coloring/handlers/ComposedColorHandler.h>
#include <api/coloring/methods/MorphologySectionColorMethod.h>
#include <api/neuron/NeuronGeometryBuilder.h>
#include <api/neuron/NeuronMorphologyPipeline.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <components/ColorHandler.h>
#include <components/NeuronSectionList.h>
#include <io/NeuronMorphologyLoaderParameters.h>

namespace
{
class SectionLoadChecker
{
public:
    static void check(const NeuronMorphologyLoaderParameters &input)
    {
        if (!input.load_axon && !input.load_dendrites)
        {
            throw std::invalid_argument("At least one section of the morphology (axons or dendrites) must be enabled");
        }
    }
};

class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model)
        : _components(model.getComponents())
        , _systems(model.getSystems())
    {
    }

    void addGeometry(std::vector<brayns::Capsule> primitives)
    {
        _components.add<brayns::Geometries>(std::move(primitives));
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setDataSystem<brayns::GeometryDataSystem>();
    }

    void addSections(std::vector<NeuronSectionMapping> mapping)
    {
        _components.add<NeuronSectionList>(std::move(mapping));
    }

    void addColoring()
    {
        _components.add<ColorHandler>(std::make_unique<ComposedColorHandler>());

        auto methods = brayns::ColorMethodList();
        methods.push_back(std::make_unique<brayns::SolidColorMethod>());
        methods.push_back(std::make_unique<MorphologySectionColorMethod>());
        _systems.setColorSystem<brayns::GenericColorSystem>(std::move(methods));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};
} // namespace

std::vector<std::string> NeuronMorphologyLoader::getSupportedExtensions() const
{
    return {"swc", "h5", "asc"};
}

std::string NeuronMorphologyLoader::getName() const
{
    return "Neuron Morphology loader";
}

std::vector<std::shared_ptr<brayns::Model>> NeuronMorphologyLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &cb,
    const NeuronMorphologyLoaderParameters &params) const
{
    (void)blob;
    (void)cb;
    (void)params;
    throw std::runtime_error("MorphologyLoader: Import from blob not supported");
}

std::vector<std::shared_ptr<brayns::Model>> NeuronMorphologyLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const NeuronMorphologyLoaderParameters &input) const
{
    brayns::Timer timer;

    auto name = getName();

    brayns::Log::info("[CE] {}: loading {}.", name, path);
    callback.updateProgress("Loading " + path, 0.f);

    SectionLoadChecker::check(input);

    auto soma = input.load_soma;
    auto axon = input.load_axon;
    auto dend = input.load_dendrites;
    NeuronMorphology morphology = NeuronMorphologyReader::read(path, soma, axon, dend);

    auto pipeline = NeuronMorphologyPipeline::fromParameters(input);
    pipeline.process(morphology);

    auto neuronBuilder = NeuronGeometryBuilder(morphology);
    auto neuronGeometry = neuronBuilder.instantiate({}, {});
    auto &primitives = neuronGeometry.geometry;
    auto &sectionMapping = neuronGeometry.sectionMapping;

    auto model = std::make_shared<brayns::Model>(ModelType::morphology);
    auto builder = ModelBuilder(*model);
    builder.addGeometry(std::move(primitives));
    builder.addSections(std::move(sectionMapping));
    builder.addColoring();

    brayns::Log::info("[CE] {}: done in {} second(s).", name, timer.seconds());

    std::vector<std::shared_ptr<brayns::Model>> result;
    result.push_back(std::move(model));
    return result;
}
