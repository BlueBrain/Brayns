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

#include "Subsampler.h"

namespace
{
class SectionSubsampler
{
public:
    using Section = NeuronMorphology::Section;
    using Sample = NeuronMorphology::SectionSample;

    explicit SectionSubsampler(uint32_t samplingFactor):
        _samplingFactor(samplingFactor)
    {
    }

    void subsample(Section &section)
    {
        if (!_isValidSection(section))
        {
            return;
        }

        auto newSamples = _allocateResult(section);
        auto &samples = section.samples;
        auto start = _samplingFactor - 1;
        auto end = samples.size() - 2;

        newSamples.push_back(samples.front());

        for (size_t i = start; i < end; i = i + _samplingFactor)
        {
            newSamples.push_back(samples[i]);
        }

        newSamples.push_back(samples.back());

        section.samples = std::move(newSamples);
    }

private:
    bool _isValidSection(const Section &section)
    {
        return section.samples.size() > 2;
    }

    std::vector<Sample> _allocateResult(const Section &section)
    {
        auto &samples = section.samples;
        auto newSamples = std::vector<Sample>();
        auto resultSize = 2 + ((samples.size() - 2) / _samplingFactor);
        newSamples.reserve(resultSize);
        return newSamples;
    }

private:
    uint32_t _samplingFactor;
};
}

Subsampler::Subsampler(uint32_t samplingFactory):
    _samplingFactor(samplingFactory)
{
}

void Subsampler::process(NeuronMorphology &morphology) const
{
    if (_samplingFactor < 2)
    {
        return;
    }

    auto subsampler = SectionSubsampler(_samplingFactor);

    for (auto &section : morphology.sections())
    {
        subsampler.subsample(section);
    }
}
