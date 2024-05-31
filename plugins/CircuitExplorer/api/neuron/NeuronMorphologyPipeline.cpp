/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "processors/ConstantRadius.h"
#include "processors/Growth.h"
#include "processors/RadiusMultiplier.h"
#include "processors/Resampler.h"
#include "processors/SectionSmoother.h"
#include "processors/Smoother.h"
#include "processors/Subsampler.h"

namespace
{
class StageBuilder
{
public:
    void addGeometryStage(NeuronGeometryType type)
    {
        switch (type)
        {
        case NeuronGeometryType::ConstantRadii:
            stages.push_back(std::make_unique<ConstantRadius>());
            break;
        case NeuronGeometryType::SectionSmooth:
            stages.push_back(std::make_unique<SectionSmoother>());
            break;
        case NeuronGeometryType::Smooth:
            stages.push_back(std::make_unique<Smoother>());
            break;
        default:
            break;
        }
    }

    void addRadiusStage(float radiusMultiplier)
    {
        if (radiusMultiplier == 1.f)
        {
            return;
        }
        stages.push_back(std::make_unique<RadiusMultiplier>(radiusMultiplier));
    }

    void addResamplingStage(float resampling)
    {
        if (resampling > 1.f)
        {
            return;
        }
        stages.push_back(std::make_unique<Resampler>(resampling));
    }

    void addSubsamplingStage(uint32_t samplingFactor)
    {
        if (samplingFactor <= 1)
        {
            return;
        }
        stages.push_back(std::make_unique<Subsampler>(samplingFactor));
    }

    void addGrowthStage(float growth)
    {
        if (growth == 1.0f)
        {
            return;
        }
        stages.push_back(std::make_unique<Growth>(growth));
    }

    NeuronMorphologyPipeline::Stages stages;
};
}

NeuronMorphologyPipeline NeuronMorphologyPipeline::fromParameters(const NeuronMorphologyLoaderParameters &parameters)
{
    auto stageBuilder = StageBuilder();
    stageBuilder.addGeometryStage(parameters.geometry_type);
    stageBuilder.addSubsamplingStage(parameters.subsampling);
    stageBuilder.addResamplingStage(parameters.resampling);
    stageBuilder.addRadiusStage(parameters.radius_multiplier);
    stageBuilder.addGrowthStage(parameters.growth);
    return NeuronMorphologyPipeline(std::move(stageBuilder.stages));
}

NeuronMorphologyPipeline::NeuronMorphologyPipeline(Stages stages):
    _stages(std::move(stages))
{
}

void NeuronMorphologyPipeline::process(NeuronMorphology &morphology) const
{
    for (auto &stage : _stages)
    {
        stage->process(morphology);
    }
}
