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

#include "PrimitiveNeuronBuilder.h"

#include <plugin/io/morphology/neuron/instances/PrimitiveNeuronInstance.h>

namespace
{
// From http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almostEqual(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x - y) <=
               std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
           // unless the result is subnormal
           || std::abs(x - y) < std::numeric_limits<T>::min();
}

class PrimitiveInstantiableGeometry : public NeuronInstantiableGeometry
{
public:
    MorphologyInstance::Ptr instantiate(
        const brayns::Vector3f& tr, const brayns::Quaternion& rot) const final
    {
        auto sphereCopy = spheres;
        for (auto& sphere : sphereCopy)
            sphere.center = tr + rot * sphere.center;

        auto coneCopy = cones;
        for (auto& cone : coneCopy)
        {
            cone.center = tr + rot * cone.center;
            cone.up = tr + rot * cone.up;
        }

        auto cylinderCopy = cylinders;
        for (auto& cylinder : cylinderCopy)
        {
            cylinder.center = tr + rot * cylinder.center;
            cylinder.up = tr + rot * cylinder.up;
        }

        return std::make_unique<PrimitiveNeuronInstance>(
            std::move(sphereCopy), std::move(cylinderCopy), std::move(coneCopy),
            data);
    }

    void addSphere(const brayns::Vector3f& c, const float r,
                   const NeuronSection section,
                   const int32_t sectionId) noexcept
    {
        const auto geomIdx = data->geometries.size();
        const auto sphereIdx = spheres.size();
        spheres.push_back(brayns::Sphere(c, r));
        data->geometries.push_back({PrimitiveType::SPHERE, sphereIdx});
        data->sectionTypeMap[section].push_back(geomIdx);
        data->sectionMap[sectionId].push_back(geomIdx);
    }

    void addCylinder(const brayns::Vector3f& c, const brayns::Vector3f& u,
                     const float r, const NeuronSection section,
                     const int32_t sectionId) noexcept
    {
        const auto geomIdx = data->geometries.size();
        const auto cylinderIdx = cylinders.size();
        cylinders.push_back(brayns::Cylinder(c, u, r));
        data->geometries.push_back({PrimitiveType::CYLINDER, cylinderIdx});
        data->sectionTypeMap[section].push_back(geomIdx);
        data->sectionMap[sectionId].push_back(geomIdx);
    }

    void addCone(const brayns::Vector3f& c, const float r,
                 const brayns::Vector3f& u, const float ru,
                 const NeuronSection section, const int32_t sectionId) noexcept
    {
        const auto geomIdx = data->geometries.size();
        const auto coneIdx = cones.size();
        cones.push_back(brayns::Cone(c, u, r, ru));
        data->geometries.push_back({PrimitiveType::CONE, coneIdx});
        data->sectionTypeMap[section].push_back(geomIdx);
        data->sectionMap[sectionId].push_back(geomIdx);
    }

    std::vector<brayns::Sphere> spheres;
    std::vector<brayns::Cylinder> cylinders;
    std::vector<brayns::Cone> cones;

    std::shared_ptr<PrimitiveSharedData> data;
};

} // namespace

NeuronInstantiableGeometry::Ptr PrimitiveNeuronBuilder::_buildImpl(
    const NeuronMorphology& m) const
{
    auto instantiableResult = std::make_unique<PrimitiveInstantiableGeometry>();
    auto& instantiable = *instantiableResult.get();

    instantiable.data = std::make_shared<PrimitiveSharedData>();

    // Add soma
    if (m.hasSoma())
    {
        const auto& soma = m.soma();
        instantiable.addSphere(soma.center, soma.radius, NeuronSection::SOMA,
                               -1);
        for (const auto child : soma.children)
        {
            if (!child->samples.empty())
                instantiable.addCone(soma.center, soma.radius,
                                     brayns::Vector3f(*child->samples.begin()),
                                     (*child->samples.begin()).w,
                                     NeuronSection::SOMA, -1);
        }
    }

    // Add dendrites and axon
    for (const auto& section : m.sections())
    {
        if (section.samples.size() > 1)
        {
            for (size_t i = 1; i < section.samples.size(); ++i)
            {
                const auto& s1 = section.samples[i - 1];
                const brayns::Vector3f p1(s1);
                const float r1 = s1.w;

                const auto& s2 = section.samples[i];
                const brayns::Vector3f p2(s2);
                const float r2 = s2.w;

                if (s1 != s2)
                {
                    if (almostEqual(r1, r2, 100000))
                        instantiable.addCylinder(p1, p2, r1, section.type,
                                                 section.id);
                    else
                        instantiable.addCone(p1, r1, p2, r2, section.type,
                                             section.id);
                }
            }
        }
        else
        {
            const auto& s1 = section.samples.front();
            const brayns::Vector3f p1(s1);
            const float r1 = s1.w;
            instantiable.addCylinder(p1, p1, r1, section.type, section.id);
        }
    }

    return instantiableResult;
}
