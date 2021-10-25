/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "PointCloudMesher.h"

#include <brayns/common/Log.h>
#include <brayns/engine/Model.h>

#include "MetaballsGenerator.h"

#if (CIRCUITEXPLORER_USE_CGAL)
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Random.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/convex_hull_3.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Point_3 Point_3;
typedef K::Segment_3 Segment_3;
typedef K::Triangle_3 Triangle_3;
#endif

PointCloudMesher::PointCloudMesher() {}

#if (CIRCUITEXPLORER_USE_CGAL)
bool PointCloudMesher::toConvexHull(brayns::Model& model,
                                    const PointCloud& pointCloud)
{
    bool addModel{false};
    for (const auto& point : pointCloud)
    {
        model.createMaterial(point.first, std::to_string(point.first));

        std::vector<Point_3> points;
        for (const auto& c : point.second)
            points.push_back({c.x(), c.y(), c.z()});

        CGAL::Object obj;
        // compute convex hull of non-collinear points
        CGAL::convex_hull_3(points.begin(), points.end(), obj);
        if (const Polyhedron_3* poly = CGAL::object_cast<Polyhedron_3>(&obj))
        {
            brayns::Log::info("[CE] The convex hull contains {} vertices.",
                              poly->size_of_vertices());

            for (auto eit = poly->edges_begin(); eit != poly->edges_end();
                 ++eit)
            {
                Point_3 a = eit->vertex()->point();
                Point_3 b = eit->opposite()->vertex()->point();
                const brayns::Cylinder cylinder(Vec3(a.x(), a.y(), a.z()),
                                                Vec3(b.x(), b.y(), b.z()), 1.f);
                model.addCylinder(point.first, cylinder);
                addModel = true;
            }
        }
        else
            brayns::Log::error("something else");
    }
    return addModel;
}
#else
bool PointCloudMesher::toConvexHull(brayns::Model&, const PointCloud&)
{
    return false;
}
#endif

#if (CIRCUITEXPLORER_USE_CGAL)
bool PointCloudMesher::toMetaballs(brayns::Model& model,
                                   const PointCloud& pointCloud,
                                   const size_t gridSize, const float threshold)
{
    auto& triangles = model.getTrianglesMeshes();
    for (const auto& point : pointCloud)
    {
        if (point.second.empty())
            continue;

        brayns::Log::info("[CE] Material {}, number of balls: {}.", point.first,
                          point.second.size());

        model.createMaterial(point.first, std::to_string(point.first));

        brayns::MetaballsGenerator metaballsGenerator;
        metaballsGenerator.generateMesh(point.second, gridSize, threshold,
                                        point.first, triangles);
    }
    return !triangles.empty();
}
#else
bool PointCloudMesher::toMetaballs(brayns::Model&, const PointCloud&,
                                   const size_t, const float)
{
    return false;
}
#endif
