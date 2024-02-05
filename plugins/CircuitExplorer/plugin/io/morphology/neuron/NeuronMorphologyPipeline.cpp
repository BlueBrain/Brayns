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

#include "NeuronMorphologyPipeline.h"

#include <plugin/io/morphology/neuron/pipeline/RadiusMultiplier.h>
#include <plugin/io/morphology/neuron/pipeline/RadiusOverride.h>
#include <plugin/io/morphology/neuron/pipeline/RadiusSmoother.h>

NeuronMorphologyPipeline NeuronMorphologyPipeline::create(
    const float radiusMultiplier, const float radiusOverride,
    const bool smooth) noexcept
{
    NeuronMorphologyPipeline pipeline;

    if (radiusOverride > 0.f)
        pipeline.registerStage<RadiusOverride>(radiusOverride);
    else if (radiusMultiplier != 1.f)
        pipeline.registerStage<RadiusMultiplier>(radiusMultiplier);

    if (smooth)
        pipeline.registerStage<RadiusSmoother>();

    return pipeline;
}

void NeuronMorphologyPipeline::process(NeuronMorphology& morphology) const
{
    for (const auto& stage : _stages)
        stage->process(morphology);
}
