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

#include "SonataEndFeetReader.h"

#include <highfive/H5File.hpp>

#include <mutex>

namespace sonataloader
{
namespace
{
constexpr char rootGroup[] = "/objects";
constexpr char endFootGroup[] = "endfoot_";
constexpr char pointDataset[] = "points";
constexpr char triangleDataset[] = "triangles";
} // namespace

std::vector<brayns::TriangleMesh> SonataEndFeetReader::readEndFeet(
    const std::string& filePath, const std::vector<uint64_t>& ids,
    const std::vector<brayns::Vector3f>& positions)
{
    static std::mutex hdf5Mutex;
    std::unique_ptr<HighFive::File> file;
    {
        std::lock_guard<std::mutex> lock(hdf5Mutex);
        file = std::make_unique<HighFive::File>(filePath);
    }

    const auto root = file->getGroup(rootGroup);

    std::vector<brayns::TriangleMesh> result(ids.size());

    for (size_t i = 0; i < ids.size(); ++i)
    {
        auto& mesh = result[i];

        const auto endFootGroupName = endFootGroup + std::to_string(ids[i]);
        const auto endFootGroup = root.getGroup(endFootGroupName);

        const auto vertexDataSet = endFootGroup.getDataSet(pointDataset);
        std::vector<std::vector<float>> rawVertices;
        vertexDataSet.select({0, 0}, vertexDataSet.getDimensions())
            .read(rawVertices);

        mesh.vertices.resize(rawVertices.size());
        for (size_t j = 0; j < rawVertices.size(); ++j)
        {
            mesh.vertices[j].x = rawVertices[j][0];
            mesh.vertices[j].y = rawVertices[j][1];
            mesh.vertices[j].z = rawVertices[j][2];
        }

        const auto triangleDataSet = endFootGroup.getDataSet(triangleDataset);
        std::vector<std::vector<uint32_t>> rawTriangles;
        triangleDataSet.select({0, 0}, triangleDataSet.getDimensions())
            .read(rawTriangles);

        mesh.indices.resize(rawTriangles.size());
        for (size_t j = 0; j < rawTriangles.size(); ++j)
        {
            mesh.indices[j].x = rawTriangles[j][0];
            mesh.indices[j].y = rawTriangles[j][1];
            mesh.indices[j].z = rawTriangles[j][2];
        }

        // Adjust mesh position given the endfoot surface position
        const auto meshBounds = brayns::createMeshBounds(mesh);
        const auto translation = meshBounds.getCenter() - positions[i];
        for (auto& vertex : mesh.vertices)
            vertex += translation;
    }

    return result;
}
} // namespace sonataloader
