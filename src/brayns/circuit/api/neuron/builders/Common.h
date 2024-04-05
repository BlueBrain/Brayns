/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/circuit/api/neuron/NeuronGeometry.h>
#include <brayns/circuit/api/neuron/NeuronMorphology.h>

#include <concepts>
#include <unordered_map>

template<typename T, typename PrimitiveType>
concept GeometryGenerationCallaback =
    std::invocable<T, const std::vector<NeuronMorphology::SectionSample> &, std::vector<PrimitiveType> &>;

/**
 * Generates the neurites geometry (through a callback) and the associated data structures for section and
 * segment mapping.
 */
class NeuriteBuilder
{
public:
    template<typename PrimitiveType, GeometryGenerationCallaback<PrimitiveType> Callback>
    static void build(
        const NeuronMorphology &morphology,
        NeuronGeometry<PrimitiveType> &geometry,
        Callback &&geometryGenerator)
    {
        auto &sectionSegments = geometry.sectionSegmentMapping;
        auto &sectionTypes = geometry.sectionTypeMapping;
        auto &primitives = geometry.primitives;

        auto &sections = morphology.sections();

        auto groupedSections = _groupSectionsByType(morphology);

        // Add section geometry, grouped by section type
        for (auto &[sectionType, sectionIndices] : groupedSections)
        {
            auto sectionTypeIndexBegin = primitives.size();

            // Add dendrites and axon
            for (auto sectionIndex : sectionIndices)
            {
                auto &section = sections[sectionIndex];
                auto &samples = section.samples;

                if (samples.empty())
                {
                    continue;
                }

                auto sectionSegmentBegin = primitives.size();
                geometryGenerator(samples, primitives);
                auto sectionSegmentEnd = primitives.size();
                sectionSegments.push_back({section.id, sectionSegmentBegin, sectionSegmentEnd});
            }

            auto sectionTypeIndexEnd = primitives.size();
            if (sectionTypeIndexEnd - sectionTypeIndexBegin > 0)
            {
                sectionTypes.push_back({sectionType, sectionTypeIndexBegin, sectionTypeIndexEnd});
            }
        }
    }

private:
    static std::unordered_map<NeuronSection, std::vector<std::size_t>> _groupSectionsByType(
        const NeuronMorphology &morphology)
    {
        std::unordered_map<NeuronSection, std::vector<std::size_t>> sortedSections;

        auto &sections = morphology.sections();
        for (std::size_t sectionIndex = 0; sectionIndex < sections.size(); ++sectionIndex)
        {
            auto &section = sections[sectionIndex];
            auto sectionType = section.type;
            auto &sectionBuffer = sortedSections[sectionType];
            sectionBuffer.push_back(sectionIndex);
        }

        return sortedSections;
    }
};

template<typename T>
concept PrimitiveAllocationSizer = std::invocable<T, const NeuronMorphology &>;
template<typename T, typename PrimitiveType>
concept PartBuilder = std::invocable<T, const NeuronMorphology &, NeuronGeometry<PrimitiveType> &>;

/**
 * Generates the neuron geometry from a neuron morphology, given a primitive counter for size allocation calculation,
 * a soma builder and a neurite builder.
 */
class NeuronBuilder
{
public:
    template<
        typename PrimitiveType,
        PrimitiveAllocationSizer Sizer,
        PartBuilder<PrimitiveType> SomaBuilder,
        PartBuilder<PrimitiveType> NeuriteBuilder>
    static NeuronGeometry<PrimitiveType> build(
        const NeuronMorphology &morphology,
        const Sizer &sizer,
        const SomaBuilder &somaBuilder,
        const NeuriteBuilder &neuriteBuilder)
    {
        auto hasSoma = morphology.hasSoma();
        auto numPrimitives = sizer(morphology);
        auto numSections = morphology.sections().size() + (hasSoma ? 1 : 0);

        auto dst = NeuronGeometry<PrimitiveType>();
        auto &primitives = dst.primitives;
        auto &sectionTypes = dst.sectionTypeMapping;
        auto &sectionSegments = dst.sectionSegmentMapping;

        primitives.reserve(numPrimitives);
        sectionTypes.reserve(4);
        sectionSegments.reserve(numSections);

        if (hasSoma)
        {
            somaBuilder(morphology, dst);
        }

        neuriteBuilder(morphology, dst);

        return dst;
    }
};
