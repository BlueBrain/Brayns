/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "SectionWelder.h"

namespace
{
struct SectionOffsets
{
    size_t offset;
};
} // namespace

void SectionWelder::process(NeuronMorphology& morphology) const
{
    for (auto& section : morphology.sections())
    {
        auto parent = morphology.parent(section);
        if (parent)
        {
            const auto& parentSample = parent->samples.back();
            const auto& childSample = section.samples.front();

            const brayns::Vector3f parentPos(parentSample);
            const brayns::Vector3f childPos(childSample);

            // If samples are further away than their combined radii,
            // Add a sample to make it reach its parent
            if (glm::length(parentPos - childPos) >
                parentSample.w + childSample.w)
                section.samples.insert(section.samples.begin(), parentSample);
        }
    }
}
