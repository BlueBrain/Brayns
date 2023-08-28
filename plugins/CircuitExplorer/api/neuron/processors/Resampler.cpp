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

#include "Resampler.h"

namespace
{
class SectionResampler
{
public:
    using Section = NeuronMorphology::Section;
    using Sample = NeuronMorphology::SectionSample;

    explicit SectionResampler(float treshold):
        _treshold(treshold)
    {
    }

    void resample(Section &section) const
    {
        if (!_validSection(section))
        {
            return;
        }

        auto newSamples = _allocateResult(section);
        auto &samples = section.samples;
        auto end = samples.size() - 1;

        for (size_t i = 1; i < end; ++i)
        {
            if (_samplesAreAligned(newSamples.back(), samples[i], samples[i + 1]))
            {
                continue;
            }
            newSamples.push_back(samples[i]);
        }

        newSamples.push_back(samples.back());
        section.samples = std::vector<Sample>(newSamples.begin(), newSamples.end());
    }

private:
    bool _validSection(const Section &section) const
    {
        return section.samples.size() > 2;
    }

    std::vector<Sample> _allocateResult(Section &section) const
    {
        auto &samples = section.samples;
        auto newSamples = std::vector<Sample>();
        newSamples.reserve(samples.size());
        newSamples.push_back(samples.front());
        return newSamples;
    }

    bool _samplesAreAligned(const Sample &pivot, const Sample &start, const Sample &end) const
    {
        auto &pivotPos = pivot.position;
        auto &startPos = start.position;
        auto &endPos = end.position;
        auto v1 = brayns::math::normalize(startPos - pivotPos);
        auto v2 = brayns::math::normalize(endPos - startPos);
        return brayns::math::dot(v1, v2) >= _treshold;
    }

private:
    float _treshold;
};
}

Resampler::Resampler(float treshold):
    _treshold(treshold)
{
}

void Resampler::process(NeuronMorphology &morphology) const
{
    if (_treshold > 1.f)
    {
        return;
    }

    auto resampler = SectionResampler(_treshold);

    for (auto &section : morphology.sections())
    {
        resampler.resample(section);
    }
}
