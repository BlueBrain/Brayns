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

#include <morphio/errorMessages.h>
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#include <filesystem>
#include <mutex>

namespace
{
morphio::Morphology readMorphology(const std::string &path)
{
    static std::mutex hdf5Mutex;
    const auto ext = std::filesystem::path(path).extension().string();
    if (ext == ".h5")
    {
        std::lock_guard<std::mutex> lock(hdf5Mutex);
        return morphio::Morphology(path);
    }
    return morphio::Morphology(path);
}

NeuronMorphology::Soma readSoma(const morphio::Morphology &m)
{
    const auto somaData = m.soma();
    const auto somaPoints = somaData.points();

    if (somaPoints.empty())
    {
        throw std::runtime_error("Morphology does not have somata information");
    }

    // Compute average position
    std::vector<brayns::Vector3f> somaSamples;
    somaSamples.reserve(somaPoints.size());
    brayns::Vector3f somaPos(0.f, 0.f, 0.f);
    for (size_t i = 0; i < somaPoints.size(); ++i)
    {
        const auto p = somaPoints[i];
        somaSamples.emplace_back(p[0], p[1], p[2]);
        somaPos.x += p[0];
        somaPos.y += p[1];
        somaPos.z += p[2];
    }
    somaPos /= static_cast<float>(somaPoints.size());

    // Compute mean radius
    float somaRadius = 0.f;
    for (const auto &somaPoint : somaSamples)
        somaRadius += glm::length(somaPoint - somaPos);
    somaRadius /= static_cast<float>(somaPoints.size());

    NeuronMorphology::Soma result;
    result.center = std::move(somaPos);
    result.radius = somaRadius;
    return result;
}

std::vector<NeuronMorphology::Section> readNeurites(const morphio::Morphology &m, const bool axon, const bool dendrites)
{
    const auto &morphSections = m.sections();

    std::vector<NeuronMorphology::Section> result;
    result.reserve(morphSections.size());

    for (const auto &section : morphSections)
    {
        const auto secPoints = section.points();
        if (secPoints.empty())
        {
            continue;
        }

        const auto secDiameters = section.diameters();
        const auto sectionId = section.id();
        const auto parentId = section.isRoot() ? -1 : section.parent().id();

        // Fill in or filter section types
        NeuronSection type;
        switch (section.type())
        {
        case morphio::SectionType::SECTION_AXON:
            if (!axon)
                continue;
            type = NeuronSection::AXON;
            break;
        case morphio::SectionType::SECTION_DENDRITE:
            if (!dendrites)
                continue;
            type = NeuronSection::DENDRITE;
            break;
        case morphio::SectionType::SECTION_APICAL_DENDRITE:
            if (!dendrites)
                continue;
            type = NeuronSection::APICAL_DENDRITE;
            break;
        default:
            continue;
        }

        result.emplace_back();
        auto &sectionObject = result.back();

        // + 1 because MorphIO returns section IDs starting at -1, whereas compartment reports begin sections at 0
        // And thats the only use we make of section IDs
        sectionObject.id = sectionId + 1;
        sectionObject.parentId = parentId;
        sectionObject.type = type;

        auto &sectionSamples = sectionObject.samples;
        sectionSamples.reserve(secPoints.size());
        for (size_t i = 0; i < secPoints.size(); ++i)
        {
            const auto &p = secPoints[i];
            sectionSamples.emplace_back();
            auto &sample = sectionSamples.back();
            sample.position = brayns::Vector3f(p[0], p[1], p[2]);
            sample.radius = secDiameters[i];
        }
    }

    return result;
}
} // namespace

// ------------------------------------------------------------------------------------------------

NeuronMorphology::NeuronMorphology(const std::string &path, const bool soma, const bool axon, const bool dendrites)
{
    if (!soma && !axon && !dendrites)
        throw std::invalid_argument("NeuronMorphology: Nothing requested to be loaded");

    // If no neurites requested, this object is useless...
    if (!axon && !dendrites)
    {
        throw std::runtime_error("Soma-only NeuronMorphologies not allowed");
    }
    else
    {
        const auto morph = readMorphology(path);
        _sections = readNeurites(morph, axon, dendrites);

        if (soma)
        {
            _soma = readSoma(morph);
            for (size_t i = 0; i < _sections.size(); ++i)
            {
                const auto &section = _sections[i];
                if (section.parentId == -1)
                {
                    _soma->childrenIndices.push_back(i);
                }
            }
        }
    }
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
    const auto checkId = section.id;
    std::vector<size_t> indices;

    for (size_t i = 0; i < _sections.size(); ++i)
    {
        const auto &candidateSection = _sections[i];
        if (candidateSection.parentId == checkId)
        {
            indices.push_back(i);
        }
    }

    return indices;
}
