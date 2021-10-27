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

#include "NeuronMorphologyImporter.h"

#include <plugin/io/morphology/neuron/builders/PrimitiveNeuronBuilder.h>
#include <plugin/io/morphology/neuron/builders/SDFNeuronBuilder.h>
#include <plugin/io/morphology/neuron/builders/SampleNeuronBuilder.h>

#include <plugin/io/morphology/neuron/pipeline/RadiusMultiplier.h>
#include <plugin/io/morphology/neuron/pipeline/RadiusOverride.h>
#include <plugin/io/morphology/neuron/pipeline/RadiusSmoother.h>

#include <unordered_map>

namespace
{
/**
 * @brief The NeuronBuilderTable class is a table to access the available neuron geometry builders
 */
class NeuronBuilderTable
{
public:
    /**
     * @brief NeuronBuilderTable constructor registers all available builders
     */
    NeuronBuilderTable()
    {
        registerBuilder<PrimitiveNeuronBuilder>();
        registerBuilder<SDFNeuronBuilder>();
        registerBuilder<SampleNeuronBuilder>();
    }

    /**
     * @brief instantiate a builder by its type and registers it on the table
     */
    template<typename T,
             typename = std::enable_if_t<std::is_base_of<NeuronBuilder, T>::value>>
    void registerBuilder()
    {
        auto builder = std::make_unique<T>();
        const auto name = builder->getName();
        _builders[name] = std::move(builder);
    }

    /**
     * @brief Search and returns a neuron builder by its name
     *
     * @throws std::runtime_error if the given name does not correspond to an
     * existing builder
     * @returns a reference to a neuron builder
     */
    const NeuronBuilder& getBuilder(const std::string& name)
    {
        auto it = _builders.find(name);
        if(it == _builders.end())
            throw std::runtime_error("NeuronBuilderTable: Unknown builder "+name);

        return *(it->second.get());
    }

    std::vector<std::string> getAvailableBuilderNames() const noexcept
    {
        std::vector<std::string> result;
        result.reserve(_builders.size());
        for(const auto& entry : _builders)
            result.push_back(entry.first);
    }
private:
    std::unordered_map<std::string, std::unique_ptr<NeuronBuilder>> _builders;
};
}

std::vector<std::string> NeuronMorphologyImporter::getAvailableGeometryTypes() noexcept
{
    return NeuronBuilderTable().getAvailableBuilderNames();
}

NeuronMorphologyImporter::NeuronMorphologyImporter(const ImportSettings& settings)
 : soma(settings.loadSoma)
 , axon(settings.loadAxon)
 , dendrites(settings.loadDendrites)
 , _builder(_getNeuronBuilder(settings.builderName))
{
    if(!soma && !axon && !dendrites)
        throw std::runtime_error("NeuronMorphologyImporter: No section enabled for loading");

    if(settings.radiusOverride > 0.f)
        _pipeline.registerStage<RadiusOverride>(settings.radiusOverride);
    else if(settings.radiusMultiplier != 1.f)
        _pipeline.registerStage<RadiusMultiplier>(settings.radiusMultiplier);

    if(settings.builderName == "smooth" && (axon || dendrites))
        _pipeline.registerStage<RadiusSmoother>();
}

NeuronInstantiableGeometry::Ptr NeuronMorphologyImporter::import(const std::string& morphologyPath) const
{
    NeuronMorphology morphology (morphologyPath, soma, axon, dendrites);
    _pipeline.process(morphology);
    return _builder.build(morphology);
}

const NeuronBuilder& NeuronMorphologyImporter::_getNeuronBuilder(const std::string& name) const
{
    static NeuronBuilderTable NEURON_BUILDER_TABLE;
    return NEURON_BUILDER_TABLE.getBuilder(name);
}
