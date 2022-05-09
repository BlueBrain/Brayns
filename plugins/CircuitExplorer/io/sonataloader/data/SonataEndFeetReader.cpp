/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

namespace
{
constexpr char dataGroup[] = "/data";
constexpr char offsetGroup[] = "/offsets";
constexpr char pointDatasetName[] = "points";
constexpr char triangleDatasetName[] = "triangles";

class DatasetExtractor
{
public:
    DatasetExtractor(const std::string &filePath)
        : _file(filePath)
        , _data(_file.getGroup(dataGroup))
        , _offsets(_file.getGroup(offsetGroup))
    {
    }

    std::vector<uint64_t> getVertexOffsets() const
    {
        return _getOffsetDataset(pointDatasetName);
    }

    std::vector<uint64_t> getTriangleOffsets() const
    {
        return _getOffsetDataset(triangleDatasetName);
    }

    std::vector<std::vector<float>> getVertices() const
    {
        return _getGeometryDataset<float>(pointDatasetName);
    }

    std::vector<std::vector<uint32_t>> getTriangles() const
    {
        return _getGeometryDataset<uint32_t>(triangleDatasetName);
    }

private:
    std::vector<uint64_t> _getOffsetDataset(std::string_view name) const
    {
        const auto offsetsDataset = _offsets.getDataSet(std::string(name));
        std::vector<uint64_t> offsets;
        auto offsetsSelection = offsetsDataset.select({0}, offsetsDataset.getDimensions());
        offsetsSelection.read(offsets);
        return offsets;
    }

    template<typename T>
    std::vector<std::vector<T>> _getGeometryDataset(std::string_view name) const
    {
        const auto geometryDataset = _data.getDataSet(std::string(name));
        std::vector<std::vector<T>> rawGeometry;
        const auto geometrySelection = geometryDataset.select({0, 0}, geometryDataset.getDimensions());
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
        const auto offsetSize = offsets.size();
        const auto currentOffset = offsets[id];

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
std::vector<brayns::TriangleMesh> SonataEndFeetReader::readEndFeet(
    const std::string &filePath,
    const std::vector<uint64_t> &ids)
{
    const auto datasetExtractor = DatasetExtractor(filePath);
    const auto vertexOffsets = datasetExtractor.getVertexOffsets();
    const auto triangleOffsets = datasetExtractor.getTriangleOffsets();
    const auto vertices = datasetExtractor.getVertices();
    const auto verticesSize = vertices.size();
    const auto triangles = datasetExtractor.getTriangles();
    const auto trianglesSize = triangles.size();

    std::vector<brayns::TriangleMesh> result(ids.size());

    for (size_t j = 0; j < ids.size(); ++j)
    {
        const auto id = ids[j];

        const auto vertexOffset = vertexOffsets[id];
        const auto vertexLength = LenghtCalculator::compute(id, vertexOffsets, verticesSize);

        const auto triangleOffset = triangleOffsets[id];
        const auto triangleLength = LenghtCalculator::compute(id, triangleOffsets, trianglesSize);

        if (vertexLength == 0 || triangleLength == 0)
        {
            continue;
        }

        auto &mesh = result[j];

        auto &meshVertices = mesh.vertices;
        meshVertices.resize(vertexLength);
        for (size_t i = 0; i < vertexLength; ++i)
        {
            const auto currentOffset = vertexOffset + i;
            meshVertices[i].x = vertices[currentOffset][0];
            meshVertices[i].y = vertices[currentOffset][1];
            meshVertices[i].z = vertices[currentOffset][2];
        }

        auto &meshIndices = mesh.indices;
        meshIndices.resize(triangleLength);
        for (size_t i = 0; i < triangleLength; ++i)
        {
            const auto faceOffset = triangleOffset + i;
            meshIndices[i].x = triangles[faceOffset][0];
            meshIndices[i].y = triangles[faceOffset][1];
            meshIndices[i].z = triangles[faceOffset][2];
        }

        brayns::TriangleMeshNormalGenerator::generate(mesh);
    }

    return result;
}
} // namespace sonataloader
