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

#include "EndFeetReader.h"

#include <highfive/H5File.hpp>

namespace
{
struct Hierarchy
{
    static inline const std::string dataGroup = "/data";
    static inline const std::string offsetGroup = "/offsets";
    static inline const std::string pointDatasetName = "points";
    static inline const std::string triangleDatasetName = "triangles";
};

class DatasetExtractor
{
public:
    explicit DatasetExtractor(const std::string &filePath):
        _file(filePath),
        _data(_file.getGroup(std::string(Hierarchy::dataGroup))),
        _offsets(_file.getGroup(std::string(Hierarchy::offsetGroup)))
    {
    }

    std::vector<uint64_t> getVertexOffsets() const
    {
        return _getOffsetDataset(Hierarchy::pointDatasetName);
    }

    std::vector<uint64_t> getTriangleOffsets() const
    {
        return _getOffsetDataset(Hierarchy::triangleDatasetName);
    }

    std::vector<std::vector<float>> getVertices() const
    {
        return _getGeometryDataset<float>(Hierarchy::pointDatasetName);
    }

    std::vector<std::vector<uint32_t>> getTriangles() const
    {
        return _getGeometryDataset<uint32_t>(Hierarchy::triangleDatasetName);
    }

private:
    std::vector<uint64_t> _getOffsetDataset(const std::string &name) const
    {
        auto offsetsDataset = _offsets.getDataSet(name);
        auto offsets = std::vector<uint64_t>();
        auto offsetsSelection = offsetsDataset.select({0}, offsetsDataset.getDimensions());
        offsetsSelection.read(offsets);
        return offsets;
    }

    template<typename T>
    std::vector<std::vector<T>> _getGeometryDataset(const std::string &name) const
    {
        auto geometryDataset = _data.getDataSet(name);
        auto rawGeometry = std::vector<std::vector<T>>();
        auto geometrySelection = geometryDataset.select({0, 0}, geometryDataset.getDimensions());
        geometrySelection.read(rawGeometry);
        return rawGeometry;
    }

private:
    HighFive::File _file;
    HighFive::Group _data;
    HighFive::Group _offsets;
};

class LenghtCalculator
{
public:
    static size_t compute(uint64_t id, const std::vector<uint64_t> &offsets, size_t rawDatasetSize)
    {
        auto offsetSize = offsets.size();
        auto currentOffset = offsets[id];

        // Its the last entry
        if (id == offsetSize - 1)
        {
            return rawDatasetSize - currentOffset;
        }

        return offsets[id + 1] - currentOffset;
    }
};
} // namespace

namespace sonataloader
{
std::vector<brayns::TriangleMesh> EndFeetReader::read(const std::string &path, const std::vector<uint64_t> &ids)
{
    auto datasetExtractor = DatasetExtractor(path);
    auto vertexOffsets = datasetExtractor.getVertexOffsets();
    auto triangleOffsets = datasetExtractor.getTriangleOffsets();
    auto vertices = datasetExtractor.getVertices();
    auto verticesSize = vertices.size();
    auto triangles = datasetExtractor.getTriangles();
    auto trianglesSize = triangles.size();

    auto result = std::vector<brayns::TriangleMesh>();
    result.reserve(ids.size());

    for (size_t j = 0; j < ids.size(); ++j)
    {
        const auto id = ids[j];

        auto vertexOffset = vertexOffsets[id];
        auto vertexLength = LenghtCalculator::compute(id, vertexOffsets, verticesSize);

        auto triangleOffset = triangleOffsets[id];
        auto triangleLength = LenghtCalculator::compute(id, triangleOffsets, trianglesSize);

        if (vertexLength == 0 || triangleLength == 0)
        {
            continue;
        }

        auto &mesh = result.emplace_back();

        auto &meshVertices = mesh.vertices;
        meshVertices.reserve(vertexLength);
        for (size_t i = 0; i < vertexLength; ++i)
        {
            auto &vertex = vertices[vertexOffset + i];
            meshVertices.emplace_back(vertex[0], vertex[1], vertex[2]);
        }

        auto &meshIndices = mesh.indices;
        meshIndices.reserve(triangleLength);
        for (size_t i = 0; i < triangleLength; ++i)
        {
            auto &triangle = triangles[triangleOffset + i];
            meshIndices.emplace_back(triangle[0], triangle[1], triangle[2]);
        }

        brayns::TriangleMeshUtils::generateNormals(mesh);
    }

    return result;
}
} // namespace sonataloader
