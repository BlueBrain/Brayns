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

#include "NeuronSphereBuilder.h"

#include "Common.h"

namespace
{
class PrimitiveAllocationSize
{
public:
    static size_t compute(const NeuronMorphology &morphology)
    {
        auto result = size_t(0);
        if (morphology.hasSoma())
        {
            auto &soma = morphology.soma();
            result += soma.samples.size();
        }
        for (const auto &section : morphology.sections())
        {
            result += section.samples.size();
        }
        return result;
    }
};

class SomaBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry<brayns::Sphere> &geometries)
    {
        auto &soma = morphology.soma();
        auto &samples = soma.samples;
        auto sampleCount = samples.size();
        for (const auto &sample : samples)
        {
            geometries.primitives.emplace_back(sample.position, sample.radius);
        }
        geometries.sectionSegmentMapping.push_back({-1, 0, sampleCount});
        geometries.sectionTypeMapping.push_back({NeuronSection::Soma, 0, sampleCount});
    }
};

class SphereNeuriteBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry<brayns::Sphere> &geometries)
    {
        NeuriteBuilder::build<brayns::Sphere>(
            morphology,
            geometries,
            [](auto &samples, auto &primitives)
            {
                for (auto &sample : samples)
                {
                    primitives.push_back(brayns::Sphere(sample.position, sample.radius));
                }
            });
    }
};

} // namespace

NeuronGeometry<brayns::Sphere> NeuronGeometryBuilder<brayns::Sphere>::build(const NeuronMorphology &morphology)
{
    return NeuronBuilder::build<brayns::Sphere>(
        morphology,
        [](auto &morphology) { return PrimitiveAllocationSize::compute(morphology); },
        [](auto &morphology, auto &geometry) { SomaBuilder::build(morphology, geometry); },
        [](auto &morphology, auto &geometry) { SphereNeuriteBuilder::build(morphology, geometry); });
}

NeuronGeometry<brayns::Sphere> NeuronGeometryInstantiator<brayns::Sphere>::instantiate(
    const NeuronGeometry<brayns::Sphere> &source,
    const brayns::Vector3f &translation,
    const brayns::Quaternion &rotation)
{
    auto copy = source;
    for (auto &sphere : copy.primitives)
    {
        sphere.center = translation + brayns::math::xfmPoint(rotation, sphere.center);
    }
    return copy;
}
