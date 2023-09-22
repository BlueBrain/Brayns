/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "SphereBuilder.h"

#include "Common.h"

namespace
{
class PrimitiveAllocationSize
{
public:
    static size_t compute(const NeuronMorphology &morphology)
    {
        size_t result = morphology.hasSoma() ? 1 : 0;
        for (const auto &section : morphology.sections())
        {
            result += section.samples.size();
        }
        return result;
    }
};

class SomaBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        const auto &soma = morphology.soma();
        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        auto somaSphere = brayns::CapsuleFactory::sphere(somaCenter, somaRadius);

        dst.primitives.push_back(somaSphere);
        dst.sectionSegmentMapping.push_back({-1, 0, 1});
        dst.sectionTypeMapping.push_back({NeuronSection::Soma, 0, 1});
    }
};

class SphereNeuriteBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        NeuriteBuilder::build(
            morphology,
            dst,
            [](const std::vector<NeuronMorphology::SectionSample> &samples, std::vector<brayns::Capsule> &primitives)
            {
                for (auto &sample : samples)
                {
                    auto sampleGeometry = brayns::CapsuleFactory::sphere(sample.position, sample.radius);
                    primitives.push_back(sampleGeometry);
                }
            });
    }
};

} // namespace

NeuronGeometryInstantiator ConnectedBuilder::build(const NeuronMorphology &morphology) const
{
    return NeuronGeometryInstantiator(NeuronBuilder::build(
        morphology,
        [](auto &morhpology) { return PrimitiveAllocationSize::compute(morhpology); },
        [](auto &morphology, auto &geometry) { SomaBuilder::build(morphology, geometry); },
        [](auto &morphology, auto &geometry) { SphereNeuriteBuilder::build(morphology, geometry); }));
}
