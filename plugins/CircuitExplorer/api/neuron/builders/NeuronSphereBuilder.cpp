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
        size_t result = morphology.hasSoma() ? 1 : 0;
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
    static void build(const NeuronMorphology &morphology, NeuronGeometry<brayns::Sphere> &dst)
    {
        const auto &soma = morphology.soma();
        const auto &somaCenter = soma.center;
        const auto somaRadius = soma.radius;
        dst.primitives.push_back(brayns::Sphere(somaCenter, somaRadius));
        dst.sectionSegmentMapping.push_back({-1, 0, 1});
        dst.sectionTypeMapping.push_back({NeuronSection::Soma, 0, 1});
    }
};

class SphereNeuriteBuilder
{
public:
    static void build(const NeuronMorphology &morphology, NeuronGeometry<brayns::Sphere> &dst)
    {
        NeuriteBuilder::build<brayns::Sphere>(
            morphology,
            dst,
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
        [](auto &morhpology) { return PrimitiveAllocationSize::compute(morhpology); },
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
