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

#include "NeuronGeometryBuilder.h"

#include <unordered_map>

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
        auto &geometry = dst.geometry;
        auto &sectionSegments = dst.sectionSegmentMapping;
        auto &sectionTypes = dst.sectionTypeMapping;

        const auto &sections = morphology.sections();
        const auto &soma = morphology.soma();

        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        auto somaSphere = brayns::CapsuleFactory::sphere(somaCenter, somaRadius);
        geometry.push_back(somaSphere);

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
            geometry.push_back(somaCone);
        }

        auto end = geometry.size();
        sectionSegments.push_back({-1, 0, end});
        sectionTypes.push_back({NeuronSection::Soma, 0, end});
    }
};

class NeuriteBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        auto &sections = morphology.sections();

        auto &geometry = dst.geometry;
        auto &sectionSegments = dst.sectionSegmentMapping;
        auto &sectionTypes = dst.sectionTypeMapping;

        // Sort sections by section type
        std::unordered_map<NeuronSection, std::vector<const NeuronMorphology::Section *>> sortedSections;
        for (const auto &section : sections)
        {
            auto sectionType = section.type;
            auto &sectionBuffer = sortedSections[sectionType];
            sectionBuffer.push_back(&section);
        }

        // Add section geometry, grouped by section type
        for (auto &[sectionType, sectionPointers] : sortedSections)
        {
            auto sectionTypeIndexBegin = geometry.size();

            // Add dendrites and axon
            for (auto sectionPtr : sectionPointers)
            {
                auto &section = *sectionPtr;
                auto &samples = section.samples;

                if (samples.empty())
                {
                    continue;
                }

                auto sectionSegmentBegin = geometry.size();

                for (size_t i = 1; i < samples.size(); ++i)
                {
                    auto &s1 = samples[i - 1];
                    auto &p1 = s1.position;
                    auto r1 = s1.radius;

                    auto &s2 = samples[i];
                    auto &p2 = s2.position;
                    auto r2 = s2.radius;

                    auto segmentGeometry = brayns::CapsuleFactory::cone(p1, r1, p2, r2);
                    geometry.push_back(segmentGeometry);
                }

                auto sectionSegmentEnd = geometry.size();
                sectionSegments.push_back({section.id, sectionSegmentBegin, sectionSegmentEnd});
            }

            auto sectionTypeIndexEnd = geometry.size();
            if (sectionTypeIndexEnd - sectionTypeIndexBegin > 0)
            {
                sectionTypes.push_back({sectionType, sectionTypeIndexBegin, sectionTypeIndexEnd});
            }
        }
    }
};

class NeuronBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry &dst)
    {
        auto hasSoma = morphology.hasSoma();
        auto numPrimitives = PrimitiveAllocationSize::compute(morphology);
        auto numSections = morphology.sections().size() + (hasSoma ? 1 : 0);

        auto &geometry = dst.geometry;
        auto &sectionTypes = dst.sectionTypeMapping;
        auto &sectionSegments = dst.sectionSegmentMapping;

        geometry.reserve(numPrimitives);
        sectionTypes.reserve(4);
        sectionSegments.reserve(numSections);

        if (hasSoma)
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
        primitive.p0 = t + brayns::math::xfmPoint(r, primitive.p0);
        primitive.p1 = t + brayns::math::xfmPoint(r, primitive.p1);
    }
    return copy;
}
