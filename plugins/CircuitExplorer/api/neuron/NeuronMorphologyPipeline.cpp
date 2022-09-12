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

#include "NeuronMorphologyPipeline.h"

#include "processors/ConstantRadius.h"
#include "processors/RadiusMultiplier.h"
#include "processors/SectionSmoother.h"
#include "processors/Smoother.h"

namespace
{
class ParametersStage
{
public:
    static NeuronMorphologyPipeline::Stages generate(NeuronGeometryType type, float radiusMultiplier)
    {
        NeuronMorphologyPipeline::Stages stages;

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

        if (radiusMultiplier != 1.f)
        {
            stages.push_back(std::make_unique<RadiusMultiplier>(radiusMultiplier));
        }

        return stages;
    }
};
}

NeuronMorphologyPipeline NeuronMorphologyPipeline::fromParameters(NeuronGeometryType type, float radiusMultiplier)
{
    return NeuronMorphologyPipeline(ParametersStage::generate(type, radiusMultiplier));
}

NeuronMorphologyPipeline::NeuronMorphologyPipeline(Stages stages)
    : _stages(std::move(stages))
{
}

void NeuronMorphologyPipeline::process(NeuronMorphology &morphology) const
{
    for (auto &stage : _stages)
    {
        stage->process(morphology);
    }
}
