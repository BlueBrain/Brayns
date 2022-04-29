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

#include "NeuronMorphologyProcessor.h"

#include <queue>

namespace
{
/**
 * @brief The RadiusMultiplier struct multiplies all morphology samples by a multiplier
 */
struct RadiusMultiplier
{
    static void apply(NeuronMorphology &morphology, const float multiplier)
    {
        if (morphology.hasSoma())
        {
            auto &soma = morphology.soma();
            soma.radius *= multiplier;
        }

        auto &sections = morphology.sections();
        for (auto &section : sections)
        {
            auto &samples = section.samples;
            for (auto &sample : samples)
            {
                sample.radius *= multiplier;
            }
        }
    }
};

/**
 * @brief The RadiusAdjuster struct adjust a sample radius based on its predecessor radius
 */
struct RadiusAdjuster
{
    static void adjustFromPrevious(std::vector<NeuronMorphology::SectionSample> &samples, size_t index)
    {
        auto &sample = samples[index];
        auto &parentSample = samples[index - 1];
        auto parentRadius = parentSample.radius;
        adjustFromRadius(sample, parentRadius);
    }

    static void adjustFromRadius(NeuronMorphology::SectionSample &sample, const float parentRadius)
    {
        // Max % of sample radius change (growth/shrink) from parent radius
        constexpr float MAX_RADIUS_PERCENT_CHANGE = 0.07f;

        auto current = sample.radius;
        const auto maxChange = parentRadius * MAX_RADIUS_PERCENT_CHANGE;
        if (std::abs(current - parentRadius) > maxChange)
        {
            if (current > parentRadius)
            {
                current = parentRadius + maxChange;
            }
            else
            {
                current = parentRadius - maxChange;
            }
        }

        const auto finalRadius = current <= 0.f ? parentRadius : current;
        sample.radius = finalRadius;
    }
};

/**
 * @brief The RadiusSmoother struct smooths morphology sample radii, starting at either soma (if loaded) or at the
 * beginning of the loaded section
 */
struct RadiusSmoother
{
    static void apply(NeuronMorphology &morphology)
    {
        // Multiplier applied to the soma radius to use it as starting radius for the sections that are born from the
        // soma
        constexpr float STARTING_SOMA_RADIUS_MULTIPLIER = 0.4f;
        std::queue<size_t> smoothQueue;

        auto &sections = morphology.sections();

        // If we have the soma, use half of its radius as starting smoothing radius
        if (morphology.hasSoma())
        {
            auto &soma = morphology.soma();
            const auto somaChildren = morphology.sectionChildrenIndices(-1);
            for (auto s : somaChildren)
            {
                auto &section = sections[s];
                auto &samples = section.samples;
                auto &firstSample = samples.front();
                firstSample.radius = soma.radius * STARTING_SOMA_RADIUS_MULTIPLIER;
                for (size_t i = 1; i < samples.size(); ++i)
                {
                    RadiusAdjuster::adjustFromPrevious(samples, i);
                }
                smoothQueue.push(s);
            }
        }
        else // Otherwise, use root sections first sample radius as starting smoothing radius
        {
            for (size_t s = 0; s < sections.size(); ++s)
            {
                auto &section = sections[s];

                // We look for root sections
                if (section.parentId != -1)
                {
                    continue;
                }

                auto &samples = section.samples;
                for (size_t i = 1; i < samples.size(); ++i)
                {
                    RadiusAdjuster::adjustFromPrevious(samples, i);
                }
                smoothQueue.push(s);
            }
        }

        // Process the rest of the sections on a parent->child manner
        while (!smoothQueue.empty())
        {
            auto sectionIndex = smoothQueue.front();
            smoothQueue.pop();

            auto &section = sections[sectionIndex];
            auto &sectionSamples = section.samples;
            auto &lastSample = sectionSamples.back();
            auto initialRadius = lastSample.radius;
            auto childrenIndices = morphology.sectionChildrenIndices(section);

            for (auto childIndex : childrenIndices)
            {
                auto &childSection = sections[childIndex];
                auto &childSamples = childSection.samples;
                float prevRadius = initialRadius;
                for (size_t i = 0; i < childSamples.size(); ++i)
                {
                    auto &sample = childSamples[i];
                    RadiusAdjuster::adjustFromRadius(sample, prevRadius);
                    prevRadius = sample.radius;
                }

                smoothQueue.push(childIndex);
            }
        }
    }
};
} // namespace

void NeuronMorphologyProcessor::processMorphology(NeuronMorphology &morphology, bool smooth, float radiusMultiplier)
{
    if (radiusMultiplier != 1.f)
    {
        RadiusMultiplier::apply(morphology, radiusMultiplier);
    }

    if (smooth)
    {
        RadiusSmoother::apply(morphology);
    }
}
