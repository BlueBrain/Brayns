/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#pragma once

#include <plugin/io/morphology/neuron/NeuronMorphology.h>

#include <type_traits>
#include <unordered_map>

/**
 * @brief The MorphologyPipelineStage class is the base class to implement
 *        morphology processing stages
 */
class NeuronMorphologyPipelineStage
{
public:
    virtual ~NeuronMorphologyPipelineStage() = default;

    /**
     * @brief applies a process to the soma and/or sections of the given
     * morphology, effectively altering it in the process
     */
    virtual void proccess(NeuronMorphology& morphology) const = 0;
};

/**
 * @brief The MorphologyPipeline class implements a configurable pipeline that
 *        allows to process a Morphology object before is being converted into
 *        a 3D shape
 */
class NeuronMorphologyPipeline
{
public:
    /**
     * @brief registers a processing stage into this pipeline. Pipeline stages
     * are stored and applied over a morphology in the same order that they were
     * added
     */
    template <typename StageClass, typename... Args>
    void registerStage(Args&&... args)
    {
        static_assert(
            std::is_base_of<NeuronMorphologyPipelineStage, StageClass>::value,
            "Attempted to register wrong type of morphology stage");

        _stages.push_back(
            std::make_unique<StageClass>(std::forward<Args>(args)...));
    }

    /**
     * @brief Process a morphology object with all the registered stages.
     * Pipeline stages are applied over a morphology in the same order that they
     * were added
     */
    void process(NeuronMorphology& morphology) const
    {
        for (const auto& stage : _stages)
            stage->proccess(morphology);
    }

private:
    std::vector<std::unique_ptr<NeuronMorphologyPipelineStage>> _stages;
};
