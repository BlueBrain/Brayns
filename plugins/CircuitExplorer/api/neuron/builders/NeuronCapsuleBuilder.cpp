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

#include "NeuronCapsuleBuilder.h"

#include "Common.h"

namespace
{
class PrimitiveAllocationSize
{
public:
    static size_t compute(const NeuronMorphology &morphology)
    {
        auto result = size_t(0);

        result += _getSomaSampleCount(morphology);

        for (const auto &section : morphology.sections())
        {
            result += _getConnectionCount(section.samples.size());
        }

        return result;
    }

private:
    static size_t _getSomaSampleCount(const NeuronMorphology &morphology)
    {
        if (!morphology.hasSoma())
        {
            return 0;
        }

        auto &soma = morphology.soma();
        auto sampleCount = soma.samples.size();

        switch (soma.type)
        {
        case NeuronMorphology::SomaType::SinglePoint:
        case NeuronMorphology::SomaType::ThreePoints:
            return 1;
        case NeuronMorphology::SomaType::Cylinders:
            return _getConnectionCount(sampleCount);
        case NeuronMorphology::SomaType::Contour:
            return _getContourSize(morphology);
        default:
            throw std::runtime_error("Internal error: corrupted soma type");
        }
    }

    static size_t _getConnectionCount(size_t sampleCount)
    {
        if (sampleCount < 2)
        {
            return 0;
        }
        return sampleCount - 1;
    }

    static size_t _getContourSize(const NeuronMorphology &morphology)
    {
        auto children = morphology.sectionChildrenIndices(-1);
        return children.size() + 1;
    }
};

class SomaBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry<brayns::Capsule> &geometries)
    {
        auto &primitives = geometries.primitives;
        auto &sectionSegments = geometries.sectionSegmentMapping;
        auto &sectionTypes = geometries.sectionTypeMapping;

        auto &soma = morphology.soma();
        auto &samples = soma.samples;

        switch (soma.type)
        {
        case NeuronMorphology::SomaType::SinglePoint:
            _singlePoint(samples, primitives);
            break;
        case NeuronMorphology::SomaType::ThreePoints:
            _threePoints(samples, primitives);
            break;
        case NeuronMorphology::SomaType::Cylinders:
            _cylinders(samples, primitives);
            break;
        case NeuronMorphology::SomaType::Contour:
            _contour(morphology, primitives);
            break;
        default:
            throw std::runtime_error("Internal error: corrupted soma type");
        }

        auto end = primitives.size();
        sectionSegments.push_back({-1, 0, end});
        sectionTypes.push_back({NeuronSection::Soma, 0, end});
    }

private:
    static void _singlePoint(
        const std::vector<NeuronMorphology::SectionSample> &samples,
        std::vector<brayns::Capsule> &capsules)
    {
        if (samples.size() != 1)
        {
            throw std::runtime_error("Internal error: corrupted single point soma");
        }
        auto &sample = samples[0];
        auto sphere = brayns::CapsuleFactory::sphere(sample.position, sample.radius);
        capsules.push_back(sphere);
    }

    static void _threePoints(
        const std::vector<NeuronMorphology::SectionSample> &samples,
        std::vector<brayns::Capsule> &capsules)
    {
        if (samples.size() != 3)
        {
            throw std::runtime_error("Internal error: corrupted 3 points soma");
        }

        auto &middle = samples[0];
        auto &center = middle.position;
        auto radius = middle.radius;

        auto top = center;
        top.y += radius;

        auto bottom = center;
        bottom.y -= radius;

        auto cylinder = brayns::CapsuleFactory::cylinder(top, bottom, radius);
        capsules.push_back(cylinder);
    }

    static void _cylinders(
        const std::vector<NeuronMorphology::SectionSample> &samples,
        std::vector<brayns::Capsule> &capsules)
    {
        auto sampleCount = samples.size();

        if (sampleCount < 2)
        {
            return;
        }

        for (auto i = size_t(1); i < sampleCount; ++i)
        {
            auto &start = samples[i - 1];
            auto &end = samples[i];

            auto cone = brayns::CapsuleFactory::cone(start.position, start.radius, end.position, end.radius);
            capsules.push_back(cone);
        }
    }

    static void _contour(const NeuronMorphology &morphology, std::vector<brayns::Capsule> &capsules)
    {
        auto &soma = morphology.soma();
        auto &samples = soma.samples;
        auto sampleCount = static_cast<float>(samples.size());

        auto center = brayns::Vector3f(0.0f);
        for (const auto &sample : samples)
        {
            center += sample.position;
        }
        center /= sampleCount;

        auto radius = 0.0f;
        for (const auto &sample : samples)
        {
            auto distance = brayns::math::length(sample.position - center);
            radius += distance;
        }
        radius /= sampleCount;

        auto sphere = brayns::CapsuleFactory::sphere(center, radius);
        capsules.push_back(sphere);

        auto &sections = morphology.sections();
        for (auto index : morphology.sectionChildrenIndices(-1))
        {
            auto &section = sections[index];
            if (section.samples.empty())
            {
                continue;
            }
            auto &child = section.samples[0];
            auto cone = brayns::CapsuleFactory::cone(center, radius, child.position, child.radius);
            capsules.push_back(cone);
        }
    }
};

class ConnectedNeuriteBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry<brayns::Capsule> &geometries)
    {
        NeuriteBuilder::build<brayns::Capsule>(
            morphology,
            geometries,
            [](const auto &samples, auto &primitives)
            {
                for (size_t i = 1; i < samples.size(); ++i)
                {
                    auto &s1 = samples[i - 1];
                    auto &p1 = s1.position;
                    auto r1 = s1.radius;

                    auto &s2 = samples[i];
                    auto &p2 = s2.position;
                    auto r2 = s2.radius;

                    primitives.push_back(brayns::CapsuleFactory::cone(p1, r1, p2, r2));
                }
            });
    }
};
} // namespace

NeuronGeometry<brayns::Capsule> NeuronGeometryBuilder<brayns::Capsule>::build(const NeuronMorphology &morphology)
{
    return NeuronBuilder::build<brayns::Capsule>(
        morphology,
        [](auto &morphology) { return PrimitiveAllocationSize::compute(morphology); },
        [](auto &morphology, auto &geometry) { SomaBuilder::build(morphology, geometry); },
        [](auto &morphology, auto &geometry) { ConnectedNeuriteBuilder::build(morphology, geometry); });
}

NeuronGeometry<brayns::Capsule> NeuronGeometryInstantiator<brayns::Capsule>::instantiate(
    const NeuronGeometry<brayns::Capsule> &source,
    const brayns::Vector3f &translation,
    const brayns::Quaternion &rotation)
{
    auto copy = source;
    for (auto &primitive : copy.primitives)
    {
        primitive.p0 = translation + brayns::math::xfmPoint(rotation, primitive.p0);
        primitive.p1 = translation + brayns::math::xfmPoint(rotation, primitive.p1);
    }
    return copy;
}
