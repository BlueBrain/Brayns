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

#include "NeuronBuilder.h"

#include <plugin/io/morphology/neuron/builders/PrimitiveNeuronBuilder.h>
#include <plugin/io/morphology/neuron/builders/SDFNeuronBuilder.h>
#include <plugin/io/morphology/neuron/builders/SampleNeuronBuilder.h>

NeuronBuilderTable::NeuronBuilderTable()
{
    registerBuilder<PrimitiveNeuronBuilder>();
    registerBuilder<SDFNeuronBuilder>();
    registerBuilder<SampleNeuronBuilder>();
}

const NeuronBuilder& NeuronBuilderTable::getBuilder(
    const std::string& name) const
{
    auto it = _builders.find(name);
    if (it == _builders.end())
        throw std::runtime_error("NeuronBuilderTable: Unknown builder " + name);

    return *(it->second.get());
}

std::vector<std::string> NeuronBuilderTable::getAvailableBuilderNames() const
    noexcept
{
    std::vector<std::string> result;
    result.reserve(_builders.size());
    for (const auto& entry : _builders)
        result.push_back(entry.first);
}
