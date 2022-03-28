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

namespace
{
void addGeometry(PrimitiveNeuronGeometry &data, int32_t sectionId, brayns::Primitive geometry)
{
    auto &geometryBuffer = data.geometry;
    auto &sectionMapping = data.sectionSegmentMapping;

    auto idx = geometryBuffer.size();
    geometryBuffer.push_back(std::move(geometry));
    sectionMapping[sectionId].push_back(idx);
}

PrimitiveNeuronGeometry build(const NeuronMorphology &morphology)
{
    PrimitiveNeuronGeometry result;
    auto &geometry = result.geometry;
    auto &sectionRanges = result.sectionMapping;

    // Add soma
    if (morphology.hasSoma())
    {
        const auto &soma = morphology.soma();
        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        auto somaSphere = brayns::Primitive::sphere(somaCenter, somaRadius);
        addGeometry(result, -1, std::move(somaSphere));

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
            addGeometry(result, -1, std::move(somaCone));
        }

        MorphologySectionMapping somaRange;
        somaRange.type = NeuronSection::SOMA;
        somaRange.begin = 0;
        somaRange.end = geometry.size();
        sectionRanges.push_back(std::move(somaRange));
    }

    // Sort sections by section type
    auto &morphologySections = morphology.sections();
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
        const auto sectionIndexBegin = geometry.size();

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
                addGeometry(result, section.id, std::move(geometry));
            }
        }

        const auto sectionIndexEnd = geometry.size();
        if(sectionIndexEnd - sectionIndexBegin > 0)
        {
            MorphologySectionMapping range;
            range.type = sectionType;
            range.begin = sectionIndexBegin;
            range.end = sectionIndexEnd;
            sectionRanges.push_back(std::move(range));
        }
    }

    return result;
}

} // namespace

PrimitiveNeuronBuilder::PrimitiveNeuronBuilder(const NeuronMorphology &morphology)
 : _data(build(morphology))
{
}

PrimitiveNeuronGeometry PrimitiveNeuronBuilder::instantiate(
        const brayns::Vector3f &position, const brayns::Quaternion &rotation) const
{
    auto copy = _data;
    auto &geometry = copy.geometry;
    for (auto &primitive : geometry)
    {
        primitive.p0 = position + rotation * primitive.p0;
        primitive.p1 = position + rotation * primitive.p1;
    }
    return copy;
}
