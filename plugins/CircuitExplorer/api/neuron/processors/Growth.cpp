/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

#include "Growth.h"

#include <cassert>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace
{
using brayns::Vector3f;
using brayns::math::length;

using Sample = NeuronMorphology::SectionSample;
using Section = NeuronMorphology::Section;
using Hierarchy = std::unordered_map<std::int32_t, std::vector<std::size_t>>;

struct SampleInfo
{
    std::size_t index;
    Vector3f position;
    float distance = 0.0f;
};

struct SectionInfo
{
    std::int32_t id;
    std::size_t index;
    std::vector<SampleInfo> samples;
    std::vector<SectionInfo> children;
    float length = 0.0f;
};

Hierarchy createHierarchy(const std::vector<Section> &sections)
{
    auto hierarchy = Hierarchy();

    for (auto i = std::size_t(0); i < sections.size(); ++i)
    {
        hierarchy[sections[i].parentId].push_back(i);
    }

    return hierarchy;
}

std::vector<SampleInfo> prepareSampleInfos(const std::vector<Sample> &samples)
{
    auto sampleCount = samples.size();

    auto infos = std::vector<SampleInfo>();
    infos.reserve(sampleCount);

    for (auto i = std::size_t(0); i < sampleCount; ++i)
    {
        infos.push_back({.index = i, .position = samples[i].position});
    }

    return infos;
}

std::vector<SectionInfo> prepareSectionInfos(
    const std::vector<Section> &sections,
    const Hierarchy &hierarchy,
    std::int32_t sectionId = -1)
{
    auto i = hierarchy.find(sectionId);

    if (i == hierarchy.end())
    {
        return {};
    }

    const auto &childIndices = i->second;

    auto infos = std::vector<SectionInfo>();
    infos.reserve(childIndices.size());

    for (auto childIndex : childIndices)
    {
        const auto &child = sections[childIndex];

        infos.push_back({
            .id = child.id,
            .index = childIndex,
            .samples = prepareSampleInfos(child.samples),
            .children = prepareSectionInfos(sections, hierarchy, child.id),
        });
    }

    return infos;
}

void computeSampleDistances(std::vector<SampleInfo> &samples, const SampleInfo *parentSample)
{
    if (samples.empty())
    {
        return;
    }

    if (parentSample == nullptr)
    {
        parentSample = &samples[0];
    }

    for (auto &sample : samples)
    {
        auto vectorToSample = sample.position - parentSample->position;
        auto distanceToSample = length(vectorToSample);

        sample.distance = parentSample->distance + distanceToSample;

        parentSample = &sample;
    }
}

void computeSectionDistances(std::vector<SectionInfo> &sections, const SampleInfo *parentSample = nullptr)
{
    for (auto &section : sections)
    {
        computeSampleDistances(section.samples, parentSample);

        if (!section.samples.empty())
        {
            parentSample = &section.samples.back();
        }

        computeSectionDistances(section.children, parentSample);
    }
}

void computeSectionLengths(std::vector<SectionInfo> &sections, float parentSampleDistance = 0.0f)
{
    for (auto &section : sections)
    {
        auto lastSampleDistance = parentSampleDistance;

        if (!section.samples.empty())
        {
            lastSampleDistance = section.samples.back().distance;
        }

        if (section.children.empty())
        {
            section.length = lastSampleDistance;
            continue;
        }

        computeSectionLengths(section.children, lastSampleDistance);

        auto compareLength = [](const auto &left, const auto &right) { return left.length < right.length; };
        auto longestBranch = std::ranges::max_element(section.children, compareLength);

        section.length = longestBranch->length;
    }
}

std::vector<SectionInfo> createSectionInfos(const std::vector<Section> &sections)
{
    auto hierarchy = createHierarchy(sections);

    auto infos = prepareSectionInfos(sections, hierarchy);

    computeSectionDistances(infos);

    computeSectionLengths(infos);

    return infos;
}

bool shouldRemoveSample(const SectionInfo &section, const SampleInfo &sample, float growth)
{
    assert(section.length != 0.0f);

    auto normalizedDistance = sample.distance / section.length;

    return normalizedDistance >= growth;
}

void removeUnbornSamples(std::vector<Section> &sections, const std::vector<SectionInfo> &infos, float growth)
{
    for (const auto &info : infos)
    {
        auto &section = sections[info.index];
        auto &samples = section.samples;

        for (const auto &sampleInfo : info.samples)
        {
            if (shouldRemoveSample(info, sampleInfo, growth))
            {
                samples.erase(samples.begin() + sampleInfo.index, samples.end());
                break;
            }
        }

        removeUnbornSamples(sections, info.children, growth);
    }
}

void removeEmptySections(std::vector<Section> &sections)
{
    auto [first, last] = std::ranges::remove_if(sections, [](auto &section) { return section.samples.empty(); });
    sections.erase(first, last);
}
}

Growth::Growth(float percentage):
    _percentage(percentage)
{
}

void Growth::process(NeuronMorphology &morphology) const
{
    auto &sections = morphology.sections();

    auto infos = createSectionInfos(sections);

    removeUnbornSamples(sections, infos, _percentage);

    removeEmptySections(sections);
}
