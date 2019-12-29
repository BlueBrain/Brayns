/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#pragma once

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFBezier.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/types.h>
#include <brayns/engineapi/Model.h>

namespace brayns
{
struct ModelData
{
    ModelData() = default;

    // std::vector move constructor is not noexcept until C++17, if we want
    // this class to be movable we have to do it by hand.
    ModelData(ModelData&& other) noexcept
        : spheres(std::move(other.spheres))
        , cylinders(std::move(other.cylinders))
        , cones(std::move(other.cones))
        , sdfBeziers(std::move(other.sdfBeziers))
        , sdfGeometries(std::move(other.sdfGeometries))
        , sdfNeighbours(std::move(other.sdfNeighbours))
        , sdfMaterials(std::move(other.sdfMaterials))
    {
    }

    void addSphere(const size_t materialId, const Sphere& sphere)
    {
        spheres[materialId].push_back(sphere);
    }

    void addCylinder(const size_t materialId, const Cylinder& cylinder)
    {
        cylinders[materialId].push_back(cylinder);
    }

    void addCone(const size_t materialId, const Cone& cone)
    {
        cones[materialId].push_back(cone);
    }

    void addSDFBezier(const size_t materialId, const SDFBezier& bezier)
    {
        sdfBeziers[materialId].push_back(bezier);
    }

    void addSDFGeometry(const size_t materialId, const SDFGeometry& geom,
                        const std::vector<size_t> neighbours)
    {
        sdfMaterials.push_back(materialId);
        sdfGeometries.push_back(geom);
        sdfNeighbours.push_back(neighbours);
    }

    void addTo(Model& model) const
    {
        for (const auto& sphere : spheres)
        {
            const auto index = sphere.first;
            model.getSpheres()[index].insert(model.getSpheres()[index].end(),
                                             sphere.second.begin(),
                                             sphere.second.end());
        }
        for (const auto& cylinder : cylinders)
        {
            const auto index = cylinder.first;
            model.getCylinders()[index].insert(
                model.getCylinders()[index].end(), cylinder.second.begin(),
                cylinder.second.end());
        }
        for (const auto& cone : cones)
        {
            const auto index = cone.first;
            model.getCones()[index].insert(model.getCones()[index].end(),
                                           cone.second.begin(),
                                           cone.second.end());
        }
        for (const auto& sdfBezier : sdfBeziers)
        {
            const auto index = sdfBezier.first;
            model.getSDFBeziers()[index].insert(
                model.getSDFBeziers()[index].end(), sdfBezier.second.begin(),
                sdfBezier.second.end());
        }
        const size_t numGeoms = sdfGeometries.size();
        std::vector<size_t> localToGlobalIndex(numGeoms, 0);

        // Add geometries to Model. We do not know the indices of the neighbours
        // yet so we leave them empty.
        for (size_t i = 0; i < numGeoms; i++)
            localToGlobalIndex[i] =
                model.addSDFGeometry(sdfMaterials[i], sdfGeometries[i], {});

        // Write the neighbours using global indices
        uint64_ts neighboursTmp;
        for (uint64_t i = 0; i < numGeoms; i++)
        {
            const uint64_t globalIndex = localToGlobalIndex[i];
            neighboursTmp.clear();

            for (auto localNeighbourIndex : sdfNeighbours[i])
                neighboursTmp.push_back(
                    localToGlobalIndex[localNeighbourIndex]);

            model.updateSDFGeometryNeighbours(globalIndex, neighboursTmp);
        }
    }

    SpheresMap spheres;
    CylindersMap cylinders;
    ConesMap cones;
    SDFBeziersMap sdfBeziers;
    std::vector<SDFGeometry> sdfGeometries;
    std::vector<std::vector<size_t>> sdfNeighbours;
    std::vector<size_t> sdfMaterials;
};
} // namespace brayns
