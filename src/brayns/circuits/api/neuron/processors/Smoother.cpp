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

#include "Smoother.h"

#include <deque>

namespace
{
class SmoothKernel
{
public:
    static float compute(float radius, float parentRadius) noexcept
    {
        if (radius == 0.f)
        {
            return parentRadius;
        }

        constexpr float maxRadiusPercentChange = 0.07f;
        auto maxChange = parentRadius * maxRadiusPercentChange;
        auto change = std::abs(radius - parentRadius);
        if (change <= maxChange)
        {
            return radius;
        }

        if (radius > parentRadius)
        {
            return parentRadius + maxChange;
        }

        return parentRadius - maxChange;
    }
};

class SmoothQueue
{
public:
    void append(const std::vector<size_t> &indices)
    {
        _smoothQueue.insert(_smoothQueue.end(), indices.begin(), indices.end());
    }

    size_t next() noexcept
    {
        auto next = _smoothQueue.front();
        _smoothQueue.pop_front();
        return next;
    }

    bool empty() const noexcept
    {
        return _smoothQueue.empty();
    }

private:
    std::deque<size_t> _smoothQueue;
};

class ParentChildrenSmoother
{
public:
    explicit ParentChildrenSmoother(float maxParentChildrenChange):
        _maxParentChildrenChange(maxParentChildrenChange)
    {
    }

    void apply(NeuronMorphology &morphology)
    {
        SmoothQueue queue;

        _initializeRootSections(morphology, queue);

        while (!queue.empty())
        {
            auto sectionIndex = queue.next();
            _smoothSection(morphology, sectionIndex);
            _initializeChildrenSections(morphology, queue, sectionIndex);
        }
    }

private:
    void _initializeRootSections(NeuronMorphology &morphology, SmoothQueue &queue)
    {
        auto somaChildren = morphology.sectionChildrenIndices(-1);
        queue.append(somaChildren);
    }

    void _initializeChildrenSections(NeuronMorphology &morphology, SmoothQueue &queue, size_t sectionIndex)
    {
        auto &sections = morphology.sections();
        auto &section = sections[sectionIndex];
        auto &lastSample = section.samples.back();
        auto lastSampleRadius = lastSample.radius;

        auto childrenIndices = morphology.sectionChildrenIndices(section);
        queue.append(childrenIndices);

        auto callback = [&](float srcRadius) { return SmoothKernel::compute(srcRadius, lastSampleRadius); };
        _initializeFirstSample(morphology, childrenIndices, callback);
    }

    template<typename Callable>
    void _initializeFirstSample(NeuronMorphology &morphology, const std::vector<size_t> &indices, Callable &callback)
    {
        auto &sections = morphology.sections();
        for (auto childIndex : indices)
        {
            auto &section = sections[childIndex];
            auto &firstSample = section.samples.front();
            firstSample.radius = callback(firstSample.radius);
        }
    }

    void _smoothSection(NeuronMorphology &morphology, size_t index)
    {
        auto &sections = morphology.sections();
        auto &section = sections[index];
        auto &samples = section.samples;
        for (size_t i = 1; i < samples.size(); ++i)
        {
            auto &sample = samples[i];
            auto &parentSample = samples[i - 1];
            sample.radius = SmoothKernel::compute(sample.radius, parentSample.radius);
        }
    }

private:
    float _maxParentChildrenChange;
};
} // namespace

void Smoother::process(NeuronMorphology &morphology) const
{
    constexpr float maxParentChildChange = 0.07f;
    auto smoother = ParentChildrenSmoother(maxParentChildChange);
    smoother.apply(morphology);
}
