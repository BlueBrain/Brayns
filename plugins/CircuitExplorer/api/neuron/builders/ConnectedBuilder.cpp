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

#include "ConnectedBuilder.h"

#include "Common.h"

namespace
{
class PrimitiveAllocationSize
{
public:
    static size_t compute(const NeuronMorphology &morphology)
    {
        size_t result = 0;

        if (morphology.hasSoma())
        {
            const auto somaChildren = morphology.sectionChildrenIndices(-1);

            result += 1; // soma sphere
            result += somaChildren.size(); // soma-neurite cones
        }

        const auto &sections = morphology.sections();
        for (const auto &section : sections)
        {
            auto &samples = section.samples;
            if (!samples.empty())
            {
                // * -> sample
                // = -> primitive added
                // section: *=*=*=*
                // Hence -1
                result += samples.size() - 1;
            }
        }

        return result;
    }
};

class SomaBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        auto &primitives = dst.primitives;
        auto &sectionSegments = dst.sectionSegmentMapping;
        auto &sectionTypes = dst.sectionTypeMapping;

        const auto &sections = morphology.sections();
        const auto &soma = morphology.soma();

        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        auto somaSphere = brayns::CapsuleFactory::sphere(somaCenter, somaRadius);
        primitives.push_back(somaSphere);

        const auto somaChildren = morphology.sectionChildrenIndices(-1);
        for (const auto childIndex : somaChildren)
        {
            const auto &child = sections[childIndex];
            if (child.samples.empty())
            {
                continue;
            }

            const auto &childFirstSample = child.samples[0];
            const auto &samplePos = childFirstSample.position;
            const auto sampleRadius = childFirstSample.radius;
            auto somaCone = brayns::CapsuleFactory::cone(somaCenter, somaRadius, samplePos, sampleRadius);
            primitives.push_back(somaCone);
        }

        auto end = primitives.size();
        sectionSegments.push_back({-1, 0, end});
        sectionTypes.push_back({NeuronSection::Soma, 0, end});
    }
};

class ConnectedNeuriteBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        NeuriteBuilder::build(
            morphology,
            dst,
            [](const auto &samples, auto &primitives)
            {
                for (size_t i = 1; i < samples.size(); ++i)
                {
                    auto &s1 = samples[i - 1];
                    auto &p1 = s1.position;
                    auto r1 = s1.radius;

                    auto &s2 = samples[i];
                    auto &p2 = s2.position;
                    auto r2 = s2.radius;

                    auto segmentGeometry = brayns::CapsuleFactory::cone(p1, r1, p2, r2);
                    primitives.push_back(segmentGeometry);
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
        [](auto &morphology, auto &geometry) { ConnectedNeuriteBuilder::build(morphology, geometry); }));
}
