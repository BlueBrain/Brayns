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

#include "SampleNeuronBuilder.h"

#include <plugin/io/morphology/neuron/instances/SampleNeuronInstance.h>

namespace
{
class SampleNeuronInstantiableGeometry : public NeuronInstantiableGeometry
{
public:
    MorphologyInstance::Ptr instantiate(const brayns::Vector3f& t,
                                        const brayns::Quaternion& r) const final
    {
        auto transformed = samples;
        for (auto& sphere : transformed)
            sphere.center = t + r * sphere.center;

        return std::make_unique<SampleNeuronInstance>(std::move(transformed),
                                                      data);
    }

    std::vector<brayns::Sphere> samples;
    std::shared_ptr<SampleSharedData> data;
};
} // namespace

NeuronInstantiableGeometry::Ptr SampleNeuronBuilder::_buildImpl(
    const NeuronMorphology& m) const
{
    auto instantiablePtr = std::make_unique<SampleNeuronInstantiableGeometry>();
    auto& instantiable = *instantiablePtr.get();

    instantiable.data = std::make_shared<SampleSharedData>();

    // Soma
    if (m.hasSoma())
    {
        const auto& soma = m.soma();
        instantiable.samples.push_back(
            brayns::Sphere(soma.center, soma.radius));
        instantiable.data->sectionTypeMap[NeuronSection::SOMA].push_back(0);
        instantiable.data->sectionMap[-1].push_back(0);
    }

    // Dendrites and axon
    for (const auto& section : m.sections())
    {
        for (const auto& sample : section.samples)
        {
            const auto idx = instantiable.samples.size();
            instantiable.samples.push_back(
                brayns::Sphere(brayns::Vector3f(sample), sample.w));
            instantiable.data->sectionTypeMap[section.type].push_back(idx);
            instantiable.data->sectionMap[section.id].push_back(idx);
        }
    }

    return instantiablePtr;
}
