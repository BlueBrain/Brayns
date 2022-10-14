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

#include "NeuronMorphologyLoader.h"

#include <brayns/utils/Log.h>
#include <brayns/utils/Timer.h>

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/neuron/NeuronGeometryBuilder.h>
#include <api/neuron/NeuronMorphologyPipeline.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <io/NeuronMorphologyLoaderParameters.h>

namespace
{
struct SectionLoadChecker
{
    static void check(const NeuronMorphologyLoaderParameters &input)
    {
        if (!input.load_axon && !input.load_dendrites)
        {
            throw std::invalid_argument("At least one section of the morphology (axons or dendrites) must be enabled");
        }
    }
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

std::vector<std::unique_ptr<brayns::Model>> NeuronMorphologyLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &cb,
    const NeuronMorphologyLoaderParameters &params) const
{
    (void)blob;
    (void)cb;
    (void)params;
    throw std::runtime_error("MorphologyLoader: Import from blob not supported");
}

std::vector<std::unique_ptr<brayns::Model>> NeuronMorphologyLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const NeuronMorphologyLoaderParameters &input) const
{
    brayns::Timer timer;

    auto name = getName();

    brayns::Log::info("[CE] {}: loading {}.", name, path);
    callback.updateProgress("Loading " + path, 0.f);

    SectionLoadChecker::check(input);

    auto radMultiplier = input.radius_multiplier;
    auto soma = input.load_soma;
    auto axon = input.load_axon;
    auto dend = input.load_dendrites;
    auto geometryType = input.geometry_type;

    NeuronMorphology morphology = NeuronMorphologyReader::read(path, soma, axon, dend);

    const auto pipeline = NeuronMorphologyPipeline::fromParameters(geometryType, radMultiplier);
    pipeline.process(morphology);

    const NeuronGeometryBuilder builder(morphology);
    auto neuronGeometry = builder.instantiate({}, {});
    auto &primitives = neuronGeometry.geometry;

    auto model = std::make_unique<brayns::Model>();

    auto &components = model->getComponents();
    auto &geometries = components.add<brayns::Geometries>();
    geometries.elements.emplace_back(std::move(primitives));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
    systems.setInitSystem<brayns::GeometryInitSystem>();
    systems.setCommitSystem<brayns::GeometryCommitSystem>();

    brayns::Log::info("[CE] {}: done in {} second(s).", name, timer.seconds());

    std::vector<std::unique_ptr<brayns::Model>> result;
    result.push_back(std::move(model));
    return result;
}
