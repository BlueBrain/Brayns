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

#include "SectionSmoother.h"

#include <deque>

namespace
{
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
    explicit ParentChildrenSmoother(float sectionRadiusChange):
        _sectionRadiusChange(sectionRadiusChange)
    {
        assert(_sectionRadiusChange >= 0.f && _sectionRadiusChange <= 1.f);
    }

    void apply(NeuronMorphology &morphology)
    {
        SmoothQueue queue;
        _initializeRootSections(morphology, queue);

        while (!queue.empty())
        {
            auto parentIndex = queue.next();
            auto &parentSection = morphology.sections()[parentIndex];
            auto parentRadius = parentSection.samples.front().radius;

            auto childSections = morphology.sectionChildrenIndices(parentSection.id);
            auto childrenRadius = parentRadius * (1.f - _sectionRadiusChange);
            _setSectionRadius(morphology, childSections, childrenRadius);

            queue.append(childSections);
        }
    }

private:
    void _initializeRootSections(NeuronMorphology &morphology, SmoothQueue &queue)
    {
        auto somaChildren = morphology.sectionChildrenIndices(-1);
        queue.append(somaChildren);
    }

    void _setSectionRadius(NeuronMorphology &morphology, const std::vector<size_t> &sectionIndices, float radius)
    {
        auto &sections = morphology.sections();
        for (auto sectionIndex : sectionIndices)
        {
            auto &section = sections[sectionIndex];
            for (auto &sample : section.samples)
            {
                sample.radius = radius;
            }
        }
    }

private:
    float _somaRadiusMultiplier;
    float _sectionRadiusChange;
};
} // namespace

void SectionSmoother::process(NeuronMorphology &morphology) const
{
    constexpr float sectionRadiusChange = 0.11f;
    auto smoother = ParentChildrenSmoother(sectionRadiusChange);
    smoother.apply(morphology);
}
