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

#include <spdlog/fmt/fmt.h>

#include <morphio/morphology.h>
#include <morphio/section.h>
#include <morphio/soma.h>

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
    static std::optional<NeuronMorphology::Soma> read(const morphio::Morphology &morphology)
    {
        auto soma = morphology.soma();

        auto originalType = soma.type();

        if (originalType == morphio::SOMA_UNDEFINED)
        {
            return std::nullopt;
        }

        auto type = _getSomaType(originalType);

        auto points = soma.points();
        auto diameters = soma.diameters();

        if (points.size() != diameters.size())
        {
            throw std::runtime_error("Corrupted morphology soma (radius count != position count)");
        }

        auto sampleCount = points.size();

        if (type == NeuronMorphology::SomaType::SinglePoint)
        {
            _checkSampleCount(sampleCount, 1);
        }

        if (type == NeuronMorphology::SomaType::ThreePoints)
        {
            _checkSampleCount(sampleCount, 3);
        }

        auto samples = std::vector<NeuronMorphology::SectionSample>();
        samples.reserve(sampleCount);

        for (auto i = size_t(0); i < sampleCount; ++i)
        {
            auto &point = points[i];
            auto &diameter = diameters[i];

            auto &sample = samples.emplace_back();
            sample.position = {point[0], point[1], point[2]};
            sample.radius = diameter / 2.0f;
        }

        return NeuronMorphology::Soma{type, std::move(samples)};
    }

private:
    static NeuronMorphology::SomaType _getSomaType(morphio::SomaType original)
    {
        switch (original)
        {
        case morphio::SOMA_SINGLE_POINT:
            return NeuronMorphology::SomaType::SinglePoint;
        case morphio::SOMA_NEUROMORPHO_THREE_POINT_CYLINDERS:
            return NeuronMorphology::SomaType::ThreePoints;
        case morphio::SOMA_CYLINDERS:
            return NeuronMorphology::SomaType::Cylinders;
        case morphio::SOMA_SIMPLE_CONTOUR:
            return NeuronMorphology::SomaType::Contour;
        default:
            throw std::runtime_error("Unknown soma type");
        }
    }

    static void _checkSampleCount(size_t sampleCount, size_t expectedCount)
    {
        if (sampleCount == expectedCount)
        {
            return;
        }
        auto message = fmt::format("Invalid sample count: {} (expected {})", sampleCount, expectedCount);
        throw std::runtime_error(message);
    }
};

class NeuriteReader
{
public:
    static std::vector<NeuronMorphology::Section> read(const morphio::Morphology &morphology, bool axon, bool dendrites)
    {
        const auto &morphSections = morphology.sections();

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

    auto morphology = MorphIOReader::read(path);

    auto sections = NeuriteReader::read(morphology, axon, dendrites);

    auto optionalSoma = std::optional<NeuronMorphology::Soma>();
    if (soma)
    {
        optionalSoma = SomaReader::read(morphology);
    }

    return NeuronMorphology(std::move(sections), std::move(optionalSoma));
}
