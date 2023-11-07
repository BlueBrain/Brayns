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

#include "NeuronMorphologyReader.h"

#include <morphio/errorMessages.h>
#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

#include <filesystem>
#include <mutex>

namespace
{
class MorphIOReader
{
public:
    static morphio::Morphology read(const std::string &path)
    {
        if (path.find(".h5") != std::string::npos)
        {
            std::lock_guard lock(_h5Mutex);
            return morphio::Morphology(path);
        }

        return morphio::Morphology(path);
    }

private:
    static inline std::mutex _h5Mutex;
};

class SomaReader
{
public:
    static NeuronMorphology::Soma read(const morphio::Morphology &m)
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
            somaRadius += brayns::math::length(somaPoint - somaPos);
        somaRadius /= static_cast<float>(somaPoints.size());

        NeuronMorphology::Soma result;
        result.center = std::move(somaPos);
        result.radius = somaRadius;
        return result;
    }
};

class NeuriteReader
{
public:
    static std::vector<NeuronMorphology::Section> read(const morphio::Morphology &m, bool axon, bool dendrites)
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
            // + 1 because MorphIO returns section IDs starting at -1, whereas compartment reports begin sections at 0
            // And thats the only use we make of section IDs
            const auto parentId = section.isRoot() ? -1 : section.parent().id() + 1;

            // Fill in or filter section types
            NeuronSection type;
            switch (section.type())
            {
            case morphio::SectionType::SECTION_AXON:
                if (!axon)
                    continue;
                type = NeuronSection::Axon;
                break;
            case morphio::SectionType::SECTION_DENDRITE:
                if (!dendrites)
                    continue;
                type = NeuronSection::Dendrite;
                break;
            case morphio::SectionType::SECTION_APICAL_DENDRITE:
                if (!dendrites)
                    continue;
                type = NeuronSection::ApicalDendrite;
                break;
            default:
                continue;
            }

            auto &sectionObject = result.emplace_back();

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
                sample.radius = std::max(secDiameters[i] * 0.5f, 0.f);
            }
        }

        return result;
    }
};
} // namespace

NeuronMorphology NeuronMorphologyReader::read(const std::string &path, bool soma, bool axon, bool dendrites)
{
    if (!soma && !axon && !dendrites)
    {
        throw std::invalid_argument("Nothing requested to be loaded");
    }

    auto morph = MorphIOReader::read(path);

    auto sections = NeuriteReader::read(morph, axon, dendrites);
    std::optional<NeuronMorphology::Soma> somaObject = std::nullopt;
    if (soma)
    {
        somaObject = SomaReader::read(morph);
    }

    return NeuronMorphology(std::move(sections), std::move(somaObject));
}
