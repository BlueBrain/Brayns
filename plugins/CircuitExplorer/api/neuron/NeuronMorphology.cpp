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

#include "NeuronMorphology.h"

NeuronMorphology::NeuronMorphology(std::vector<Section> sections, std::optional<Soma> soma)
    : _sections(std::move(sections))
    , _soma(std::move(soma))
{
}

bool NeuronMorphology::hasSoma() const noexcept
{
    return _soma.has_value();
}

NeuronMorphology::Soma &NeuronMorphology::soma()
{
    try
    {
        return _soma.value();
    }
    catch (...)
    {
        throw std::runtime_error("Morphology loaded without soma");
    }
}

const NeuronMorphology::Soma &NeuronMorphology::soma() const
{
    try
    {
        return _soma.value();
    }
    catch (...)
    {
        throw std::runtime_error("Morphology loaded without soma");
    }
}

std::vector<NeuronMorphology::Section> &NeuronMorphology::sections() noexcept
{
    return _sections;
}

const std::vector<NeuronMorphology::Section> &NeuronMorphology::sections() const noexcept
{
    return _sections;
}

std::vector<size_t> NeuronMorphology::sectionChildrenIndices(const Section &section) const noexcept
{
    return sectionChildrenIndices(section.id);
}

std::vector<size_t> NeuronMorphology::sectionChildrenIndices(int32_t sectionId) const noexcept
{
    std::vector<size_t> indices;

    for (size_t i = 0; i < _sections.size(); ++i)
    {
        const auto &candidateSection = _sections[i];
        if (candidateSection.parentId == sectionId)
        {
            indices.push_back(i);
        }
    }

    return indices;
}
