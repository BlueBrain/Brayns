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

#include "NeuronMorphologyLoader.h"

#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <plugin/io/NeuronMorphologyLoaderParameters.h>
#include <plugin/io/morphology/neuron/NeuronBuilder.h>
#include <plugin/io/morphology/neuron/NeuronMorphologyPipeline.h>

namespace
{
void checkInput(const NeuronMorphologyLoaderParameters& input)
{
    if (!input.load_soma && !input.load_axon && !input.load_dendrites)
        throw std::invalid_argument(
            "NeuronMorphologyLoader: At least one section of the morphology "
            "must be enabled for loading");

    if (input.radius_override < 0.f)
        throw std::invalid_argument(
            "NeuronMorphologyLoader: 'radius_override' must be greater or "
            "equal to 0.0");
    else if (input.radius_override == 0.f && input.radius_multiplier <= 0.f)
        throw std::invalid_argument(
            "NeuronMorphologyLoader: 'radius_multiplier' must be greater than "
            "0.0");
}
} // namespace

std::vector<std::string> NeuronMorphologyLoader::getSupportedExtensions() const
{
    return {"swc", "h5", "asc"};
}

std::string NeuronMorphologyLoader::getName() const
{
    return "Neuron Morphology loader";
}

std::vector<brayns::ModelDescriptorPtr> NeuronMorphologyLoader::importFromBlob(
    brayns::Blob&&, const brayns::LoaderProgress&,
    const NeuronMorphologyLoaderParameters&, brayns::Scene&) const
{
    throw std::runtime_error(
        "MorphologyLoader: Import from blob not supported");
}

std::vector<brayns::ModelDescriptorPtr> NeuronMorphologyLoader::importFromFile(
    const std::string& path, const brayns::LoaderProgress& callback,
    const NeuronMorphologyLoaderParameters& input, brayns::Scene& scene) const
{
    brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);
    callback.updateProgress("Loading " + path, 0.f);

    checkInput(input);

    const auto& geometryMode = input.geometry_mode;
    const auto radMultiplier = input.radius_multiplier;
    const auto radOverride = input.radius_override;
    const auto loadSoma = input.load_soma;
    const auto loadAxon = input.load_axon;
    const auto loadDend = input.load_dendrites;

    const NeuronBuilder& builder =
        NeuronBuilderTable().getBuilder(geometryMode);
    const NeuronMorphologyPipeline pipeline =
        NeuronMorphologyPipeline::create(radMultiplier, radOverride,
                                         geometryMode == "smooth" &&
                                             (loadAxon || loadDend));

    NeuronMorphology morphology(path, loadSoma, loadAxon, loadDend);
    pipeline.process(morphology);
    const auto instantiable = builder.build(morphology);

    const auto geometry =
        instantiable->instantiate(brayns::Vector3f(), brayns::Quaternion());

    auto modelPtr = scene.createModel();
    geometry->addToModel(*modelPtr);

    modelPtr->updateBounds();
    brayns::Transformation transformation;
    transformation.setRotationCenter(modelPtr->getBounds().getCenter());

    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                  "Morphology", path,
                                                  brayns::ModelMetadata());
    modelDescriptor->setTransformation(transformation);

    brayns::Log::info("[CE] {}: done in {} second(s).", timer.elapsed());

    return {modelDescriptor};
}
