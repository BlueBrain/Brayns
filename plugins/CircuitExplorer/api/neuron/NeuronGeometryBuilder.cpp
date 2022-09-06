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

#include "NeuronGeometryBuilder.h"

namespace
{
struct PrimitiveAllocationSize
{
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

struct NeuronSectionMappingGenerator
{
    static void generate(NeuronGeometry &dst, NeuronSection section, size_t start, size_t end)
    {
        auto &sectionRanges = dst.sectionMapping;
        auto &sectionRange = sectionRanges.emplace_back();
        sectionRange.type = section;
        sectionRange.begin = start;
        sectionRange.end = end;
    }
};

struct NeuronGeometryAppender
{
    static void append(NeuronGeometry &dst, int32_t section, brayns::Capsule geometry)
    {
        auto &geometryBuffer = dst.geometry;
        auto &sectionMapping = dst.sectionSegmentMapping;

        auto idx = geometryBuffer.size();
        geometryBuffer.push_back(std::move(geometry));
        sectionMapping[section].push_back(idx);
    }
};

struct SomaBuilder
{
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        const auto &sections = morphology.sections();
        const auto &soma = morphology.soma();

        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        auto somaSphere = brayns::CapsuleFactory::sphere(somaCenter, somaRadius);
        NeuronGeometryAppender::append(dst, -1, std::move(somaSphere));

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
            NeuronGeometryAppender::append(dst, -1, std::move(somaCone));
        }

        auto &geometry = dst.geometry;
        auto end = geometry.size();
        NeuronSectionMappingGenerator::generate(dst, NeuronSection::Soma, 0, end);
    }
};

struct NeuriteBuilder
{
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        const auto &sections = morphology.sections();

        auto &geometry = dst.geometry;

        // Sort sections by section type
        std::unordered_map<NeuronSection, std::vector<const NeuronMorphology::Section *>> sortedSections;
        for (const auto &section : sections)
        {
            auto sectionType = section.type;
            auto &sectionBuffer = sortedSections[sectionType];
            sectionBuffer.push_back(&section);
        }

        // Add section geometry, grouped by section type
        for (const auto &[sectionType, sectionPointers] : sortedSections)
        {
            const auto sectionIndexBegin = geometry.size();

            // Add dendrites and axon
            for (const auto sectionPtr : sectionPointers)
            {
                const auto &section = *sectionPtr;
                const auto &samples = section.samples;

                if (samples.empty())
                {
                    continue;
                }

                for (size_t i = 1; i < samples.size(); ++i)
                {
                    const auto &s1 = samples[i - 1];
                    const auto &p1 = s1.position;
                    const auto r1 = s1.radius;

                    const auto &s2 = samples[i];
                    const auto &p2 = s2.position;
                    const auto r2 = s2.radius;

                    auto segmentGeometry = brayns::CapsuleFactory::cone(p1, r1, p2, r2);
                    NeuronGeometryAppender::append(dst, section.id, std::move(segmentGeometry));
                }
            }

            const auto sectionIndexEnd = geometry.size();
            if (sectionIndexEnd - sectionIndexBegin > 0)
            {
                NeuronSectionMappingGenerator::generate(dst, sectionType, sectionIndexBegin, sectionIndexEnd);
            }
        }
    }
};

struct NeuronBuilder
{
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        auto numPrimitives = PrimitiveAllocationSize::compute(morphology);
        auto &geometry = dst.geometry;
        auto &sectionRanges = dst.sectionMapping;

        geometry.reserve(numPrimitives);
        sectionRanges.reserve(4);

        if (morphology.hasSoma())
        {
            SomaBuilder::build(morphology, dst);
        }

        NeuriteBuilder::build(morphology, dst);
    }
};
} // namespace

NeuronGeometryBuilder::NeuronGeometryBuilder(const NeuronMorphology &morphology)
{
    NeuronBuilder::build(morphology, _data);
}

NeuronGeometry NeuronGeometryBuilder::instantiate(const brayns::Vector3f &t, const brayns::Quaternion &r) const
{
    auto copy = _data;
    auto &geometry = copy.geometry;
    for (auto &primitive : geometry)
    {
        primitive.p0 = t + r * primitive.p0;
        primitive.p1 = t + r * primitive.p1;
    }
    return copy;
}
