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

#include "SDFNeuronBuilder.h"

#include <plugin/io/morphology/neuron/instances/SDFNeuronInstance.h>

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

class SDFInstantiableGeometry : public NeuronInstantiableGeometry
{
public:
    MorphologyInstance::Ptr instantiate(
        const brayns::Vector3f& tr, const brayns::Quaternion& rot) const final
    {
        auto transformedSDF = sdfGeometries;
        for (auto& geometry : transformedSDF)
        {
            geometry.p0 = tr + (rot * geometry.p0);
            geometry.p1 = tr + (rot * geometry.p1);
        }

        return std::make_unique<SDFNeuronInstance>(std::move(transformedSDF),
                                                   sdfData);
    }

    std::vector<brayns::SDFGeometry> sdfGeometries;
    std::shared_ptr<SDFSharedData> sdfData;
};

} // namespace

class InternalBuilder
{
public:
    InternalBuilder(const NeuronMorphology& morph)
        : m(morph)
    {
        // Soma
        if (m.hasSoma())
            _addSomaGeometry();

        // Dendrites and axon
        for (const auto& section : m.sections())
        {
            std::vector<size_t> sectionGeoms;
            sectionGeoms.reserve(section.samples.size());

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
                        sectionGeoms.push_back(_addSegment(p2, r2, p1, r1,
                                                           section.id,
                                                           section.type));
                }
            }
            else
            {
                const auto& s1 = section.samples.front();
                const brayns::Vector3f p1(s1);
                const float r1 = s1.w;
                sectionGeoms.push_back(
                    _addSegment(p1, r1, p1, r1, section.id, section.type));
            }

            // Update geometry hierarchy use to merge bifurcations at the end
            const auto parentId =
                section.parentId == -1 ? section.id : section.parentId;
            _sectionHierarchy[parentId].push_back(sectionGeoms.front());
            _sectionHierarchy[section.id].push_back(sectionGeoms.back());
        }

        connectSDFBifurcations();
    }

    // List of geometries
    std::vector<brayns::SDFGeometry> _geometry;
    // List of geometry neighbours
    std::vector<std::set<uint64_t>> _neighbours;
    // Map of section id to all the geometries that forms it
    std::unordered_map<int32_t, std::vector<size_t>> _sectionMap;
    // Map each geometry to its section type
    std::vector<NeuronSection> _sectionTypeMap;

private:
    const NeuronMorphology& m;
    // Maps bifurcation section parent to the geometry indices taking
    // part in surch bifurcation, so they can be linked together
    std::unordered_map<int32_t, std::vector<size_t>> _sectionHierarchy;

private:
    size_t _addSDFGeometry(brayns::SDFGeometry&& geometry,
                           const int32_t section, const NeuronSection type)
    {
        const size_t idx = _geometry.size();
        _geometry.push_back(geometry);
        _neighbours.emplace_back();
        _sectionTypeMap.push_back(type);
        _sectionMap[section].push_back(idx);
        return idx;
    }

    void _addSomaGeometry()
    {
        const auto& soma = m.soma();

        if (!soma.children.empty())
        {
            std::set<size_t> childGeomIndices;
            for (const auto child : soma.children)
            {
                const auto& startSample = child->samples.front();
                const brayns::Vector3f samplePos(startSample);
                const float sampleRadius = startSample.w;
                auto geometry =
                    brayns::createSDFConePillSigmoid(soma.center, samplePos,
                                                     soma.radius, sampleRadius);
                const size_t geomIdx = _addSDFGeometry(std::move(geometry), -1,
                                                       NeuronSection::SOMA);
                childGeomIndices.insert(geomIdx);

                // Add geometry to hierarchy to blend later
                _sectionHierarchy[child->id].push_back(geomIdx);
            }

            for (const size_t c : childGeomIndices)
            {
                _neighbours[c] = childGeomIndices;
                _neighbours[c].erase(c);
            }
        }
        else
        {
            _addSDFGeometry(brayns::createSDFSphere(soma.center, soma.radius),
                            -1, NeuronSection::SOMA);
        }
    }

    size_t _addSegment(const brayns::Vector3f& pos, const float radius,
                       const brayns::Vector3f& target, const float targetRadius,
                       const int32_t section, const NeuronSection type)
    {
        brayns::SDFGeometry geom =
            (almostEqual(radius, targetRadius, 100000))
                ? brayns::createSDFPill(pos, target, radius)
                : brayns::createSDFConePill(pos, target, radius, targetRadius);

        return _addSDFGeometry(std::move(geom), section, type);
    }

    void connectSDFBifurcations()
    {
        for (const auto& entry : _sectionHierarchy)
        {
            for (const auto geomIdx : entry.second)
            {
                auto& geomNeighbours = _neighbours[geomIdx];
                for (const auto neighIdx : entry.second)
                    geomNeighbours.insert(neighIdx);
                geomNeighbours.erase(geomIdx);
            }
        }
    }
};

NeuronInstantiableGeometry::Ptr SDFNeuronBuilder::_buildImpl(
    const NeuronMorphology& m) const
{
    auto instantiablePtr = std::make_unique<SDFInstantiableGeometry>();
    auto& instantiable = *instantiablePtr.get();

    InternalBuilder builder(m);

    instantiable.sdfGeometries = std::move(builder._geometry);

    instantiable.sdfData = std::make_shared<SDFSharedData>();
    instantiable.sdfData->neighbours.resize(instantiable.sdfGeometries.size());
    for (size_t i = 0; i < instantiable.sdfGeometries.size(); ++i)
    {
        // Copy set neighbours into final vector neighbour
        const auto& tn = builder._neighbours[i];
        instantiable.sdfData->neighbours[i] =
            std::vector<size_t>(tn.begin(), tn.end());
    }
    // Group geometries by section type
    for (size_t i = 0; i < builder._sectionTypeMap.size(); ++i)
        instantiable.sdfData->sectionTypeMap[builder._sectionTypeMap[i]]
            .push_back(i);

    instantiable.sdfData->sectionGeometries = std::move(builder._sectionMap);

    return instantiablePtr;
}
