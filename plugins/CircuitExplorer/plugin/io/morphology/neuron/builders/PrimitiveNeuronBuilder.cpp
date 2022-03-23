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

#include "PrimitiveNeuronBuilder.h"

#include <plugin/io/morphology/neuron/instances/PrimitiveNeuronInstance.h>

namespace
{
struct PrimitiveInstantiableGeometry : public NeuronInstantiableGeometry
{
    std::unique_ptr<MorphologyInstance> instantiate(const brayns::Vector3f &tr,
                                                    const brayns::Quaternion &rot) const final
    {
        auto geometryCopy = geometry;
        for (auto &primitive : geometryCopy)
        {
            primitive.p0 = tr + rot * primitive.p0;
            primitive.p1 = tr + rot * primitive.p1;
        }

        return std::make_unique<PrimitiveNeuronInstance>(std::move(geometryCopy), std::move(data));
    }

    void add(brayns::Primitive primitive, const int32_t sectionId) noexcept
    {
        const auto geomIdx = geometry.size();
        geometry.push_back(std::move(primitive));
        data.sectionMap[sectionId].push_back(geomIdx);
    }

    std::vector<brayns::Primitive> geometry;
    NeuronGeometryMapping data;
};

} // namespace

PrimitiveNeuronBuilder::PrimitiveNeuronBuilder()
    : NeuronBuilder("smooth")
{
}

std::unique_ptr<NeuronInstantiableGeometry> PrimitiveNeuronBuilder::build(const NeuronMorphology &m) const
{
    auto instantiableResult = std::make_unique<PrimitiveInstantiableGeometry>();
    auto &instantiable = *instantiableResult;
    auto &data = instantiable.data;
    auto &sectionRanges = data.sectionRanges;

    // Add soma
    if (m.hasSoma())
    {
        const auto &soma = m.soma();
        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        auto somaSphere = brayns::Primitive::sphere(somaCenter, somaRadius);
        instantiable.add(std::move(somaSphere), -1);

        for (const auto childPtr : soma.children)
        {
            const auto &child = *childPtr;
            if(child.samples.empty())
            {
                continue;
            }

            const auto &childFirstSample = child.samples[0];
            const brayns::Vector3f samplePos (childFirstSample);
            const float sampleRadius = childFirstSample.w;
            auto somaCone = brayns::Primitive::cone(somaCenter, somaRadius, samplePos, sampleRadius);
            instantiable.add(somaCone, -1);
        }

        NeuronSectionRange somaRange;
        somaRange.section = NeuronSection::SOMA;
        somaRange.begin = 0;
        somaRange.end = instantiable.geometry.size();
        sectionRanges.push_back(std::move(somaRange));
    }

    // Sort sections by section type
    auto &morphologySections = m.sections();
    std::unordered_map<NeuronSection, std::vector<const NeuronMorphology::Section*>> sortedSections;
    for(const auto &section : morphologySections)
    {
        auto sectionType = section.type;
        // We've already handled soma
        if(sectionType == NeuronSection::SOMA)
        {
            continue;
        }

        auto &sectionBuffer = sortedSections[sectionType];
        sectionBuffer.push_back(&section);
    }

    // Add section geometry, grouped by section type
    for(const auto& [sectionType, sectionPointers] : sortedSections)
    {
        const auto sectionIndexBegin = instantiable.geometry.size();

        // Add dendrites and axon
        for (const auto sectionPtr : sectionPointers)
        {
            const auto &section = *sectionPtr;
            const auto &sectionSamples = section.samples;

            if(sectionSamples.empty())
            {
                continue;
            }

            for (size_t i = 1; i < section.samples.size(); ++i)
            {
                const auto &s1 = section.samples[i - 1];
                const brayns::Vector3f p1(s1);
                const float r1 = s1.w;

                const auto &s2 = section.samples[i];
                const brayns::Vector3f p2(s2);
                const float r2 = s2.w;

                const auto geometry = brayns::Primitive::cone(p1, r1, p2, r2);
                instantiable.add(std::move(geometry), section.id);
            }
        }

        const auto sectionIndexEnd = instantiable.geometry.size();
        if(sectionIndexEnd - sectionIndexBegin > 0)
        {
            NeuronSectionRange range;
            range.section = sectionType;
            range.begin = sectionIndexBegin;
            range.end = sectionIndexEnd;
            sectionRanges.push_back(std::move(range));
        }
    }

    return instantiableResult;
}
