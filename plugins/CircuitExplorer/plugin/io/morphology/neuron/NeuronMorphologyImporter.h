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

#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/morphology/neuron/NeuronBuilder.h>
#include <plugin/io/morphology/neuron/NeuronMorphologyPipeline.h>

/**
 * @brief The NeuronMorphologyImporter class loads, process and builds a
 * morphology geometry
 */
class NeuronMorphologyImporter
{
public:
    /**
     * @brief getAvailableGeometryTypes returns the names of all available
     * geomtry builder classes
     */
    static std::vector<std::string> getAvailableGeometryTypes() noexcept;

    /**
     * @brief The ImportSettings struct contains the data to configure
     * a morphology importer
     */
    struct ImportSettings
    {
        bool loadSoma{false};
        bool loadAxon{false};
        bool loadDendrites{false};
        std::string builderName;
        float radiusMultiplier{1.f};
        float radiusOverride{0.f};
    };

    /**
     * @brief constructs an importer with the given configuration
     * @throws std::runtime_error if the choosen builder does not exist,
     * or if the settings do not request any morphology section (soma, axon or
     * dendrite)
     */
    NeuronMorphologyImporter(const ImportSettings& settings);

    /**
     * @brief loads a morphology, processes it through the pipeline and then
     * builds the geometry. Returns an NeuronInstantiableGeometry object.
     */
    NeuronInstantiableGeometry::Ptr import(
        const std::string& morphologyPath) const;

private:
    const NeuronBuilder& _getNeuronBuilder(const std::string& name) const;

    const bool soma{false};
    const bool axon{false};
    const bool dendrites{false};
    const NeuronBuilder& _builder;
    NeuronMorphologyPipeline _pipeline;
};
