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

#include "NeuronMorphology.h"

#include <brayns/utils/Filesystem.h>

#include <morphio/errorMessages.h>
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#include <mutex>

namespace
{
morphio::Morphology readMorphology(const std::string& path)
{
    static std::mutex hdf5Mutex;
    const auto ext = fs::path(path).extension().string();
    if (ext == ".h5")
    {
        std::lock_guard<std::mutex> lock(hdf5Mutex);
        return morphio::Morphology(path);
    }
    return morphio::Morphology(path);
}

std::unique_ptr<NeuronMorphology::Soma> readSoma(const morphio::Morphology& m)
{
    const auto somaData = m.soma();
    const auto somaPoints = somaData.points();

    if (somaPoints.empty())
        return {nullptr};

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
    for (const auto& somaPoint : somaSamples)
        somaRadius += glm::length(somaPoint - somaPos);
    somaRadius /= static_cast<float>(somaPoints.size());

    return std::make_unique<NeuronMorphology::Soma>(somaPos, somaRadius);
}

std::vector<NeuronMorphology::Section> readNeurites(
    const morphio::Morphology& m, const bool axon, const bool dendrites)
{
    if (!axon && !dendrites)
        return {};

    const auto& morphSections = m.sections();

    std::vector<NeuronMorphology::Section> result;
    result.reserve(morphSections.size());

    for (const auto& section : morphSections)
    {
        const auto secPoints = section.points();
        if (secPoints.empty())
            continue;

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
            break;
        }

        result.emplace_back(sectionId, parentId, type);
        NeuronMorphology::Section& resultSection = result.back();

        // Fill in points
        for (size_t i = 0; i < secPoints.size(); ++i)
        {
            const auto& p = secPoints[i];
            resultSection.samples.emplace_back(p[0], p[1], p[2],
                                               secDiameters[i]);
        }
    }

    return result;
}
} // namespace

// ------------------------------------------------------------------------------------------------

NeuronMorphology::NeuronMorphology(const std::string& path, const bool soma,
                                   const bool axon, const bool dendrites)
    : _morphologyPath(path)
    , _soma(nullptr)
{
    if (!soma && !axon && !dendrites)
        throw std::invalid_argument(
            "NeuronMorphology: Nothing requested to be loaded");

    // If only soma requested, do not load the morphology file
    if (soma && !axon && !dendrites)
        _soma = std::make_unique<Soma>(brayns::Vector3f(0.f, 0.f, 0.f), 1.f);
    else
    {
        const auto morph = readMorphology(path);
        _sections = readNeurites(morph, axon, dendrites);

        if (soma && (_soma = readSoma(morph)))
        {
            for (auto& sec : _sections)
            {
                if (sec.parentId == -1)
                    _soma->children.push_back(&sec);
            }
        }
    }
}

bool NeuronMorphology::hasSoma() const noexcept
{
    return (_soma.get() != nullptr);
}

NeuronMorphology::Soma& NeuronMorphology::soma()
{
    if (hasSoma())
        return *_soma;

    throw std::runtime_error("Morphology " + _morphologyPath +
                             " loaded without soma");
}

const NeuronMorphology::Soma& NeuronMorphology::soma() const
{
    if (hasSoma())
        return *_soma;

    throw std::runtime_error("Morphology " + _morphologyPath +
                             " loaded without soma");
}

std::vector<NeuronMorphology::Section>& NeuronMorphology::sections() noexcept
{
    return _sections;
}

const std::vector<NeuronMorphology::Section>& NeuronMorphology::sections() const
    noexcept
{
    return _sections;
}

std::vector<NeuronMorphology::Section*> NeuronMorphology::sectionChildren(
    const Section& parent) noexcept
{
    std::vector<Section*> result;
    for (auto& section : _sections)
    {
        if (section.parentId == parent.id)
            result.push_back(&section);
    }
    return result;
}

std::vector<const NeuronMorphology::Section*> NeuronMorphology::sectionChildren(
    const Section& parent) const noexcept
{
    std::vector<const Section*> result;
    for (const auto& section : _sections)
    {
        if (section.parentId == parent.id)
            result.push_back(&section);
    }
    return result;
}

NeuronMorphology::Section* NeuronMorphology::parent(
    const Section& section) noexcept
{
    for (auto& parent : _sections)
    {
        if (parent.id == section.parentId)
            return &parent;
    }

    return nullptr;
}

const NeuronMorphology::Section* NeuronMorphology::parent(
    const Section& section) const noexcept
{
    for (auto& parent : _sections)
    {
        if (parent.id == section.parentId)
            return &parent;
    }

    return nullptr;
}

// ------------------------------------------------------------------------------------------------

NeuronMorphology::Section::Section(const int32_t idParam,
                                   const int32_t parentIdParam,
                                   const NeuronSection typeParam)
    : id(idParam)
    , parentId(parentIdParam)
    , type(typeParam)
{
}

// ------------------------------------------------------------------------------------------------

NeuronMorphology::Soma::Soma(const brayns::Vector3f& centerParam,
                             const float radiusParam)
    : center(centerParam)
    , radius(radiusParam)
{
}
