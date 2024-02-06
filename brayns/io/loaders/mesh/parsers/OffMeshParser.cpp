/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "OffMeshParser.h"

#include <brayns/utils/parsing/FileStream.h>
#include <brayns/utils/parsing/Parser.h>
#include <brayns/utils/parsing/ParsingException.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringTrimmer.h>

namespace
{
using namespace brayns;

struct MeshBuffer
{
    std::vector<Vector3f> vertices;
    std::vector<Vector3ui> indices;
};

struct Dimensions
{
    size_t vertexCount = 0;
    size_t faceCount = 0;
    size_t edgeCount = 0;
};

class LineFormatter
{
public:
    static std::string_view format(std::string_view data)
    {
        auto result = StringExtractor::extractUntil(data, '#');
        return StringTrimmer::trim(result);
    }
};

class LineExtractor
{
public:
    static std::string_view nextLine(FileStream &stream)
    {
        while (true)
        {
            if (!stream.nextLine())
            {
                return {};
            }
            auto line = stream.getLine();
            line = LineFormatter::format(line);
            if (!line.empty())
            {
                return line;
            }
        }
        throw std::runtime_error("Internal error");
    }
};

class HeaderParser
{
public:
    static void skipHeaderIfPresent(FileStream &stream)
    {
        auto line = LineExtractor::nextLine(stream);
        if (line.empty())
        {
            throw std::runtime_error("Empty file");
        }
        if (line != "OFF")
        {
            return;
        }
        line = LineExtractor::nextLine(stream);
        if (line.empty())
        {
            throw std::runtime_error("Empty file after OFF header");
        }
    }
};

class DimensionsParser
{
public:
    static Dimensions parse(std::string_view line)
    {
        auto count = StringCounter::countTokens(line);
        if (count != 3)
        {
            throw std::runtime_error("Invalid dimensions, expected 3 tokens, got " + std::to_string(count));
        }
        Dimensions dimensions;
        Parser::extractToken(line, dimensions.vertexCount);
        Parser::extractToken(line, dimensions.faceCount);
        Parser::extractToken(line, dimensions.edgeCount);
        return dimensions;
    }
};

class VertexParser
{
public:
    static std::vector<Vector3f> parseAll(FileStream &stream, size_t count)
    {
        std::vector<Vector3f> vertices;
        vertices.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            auto line = LineExtractor::nextLine(stream);
            if (line.empty())
            {
                auto message = "Not enough vertices, expected " + std::to_string(count) + ", got " + std::to_string(i);
                throw std::runtime_error(message);
            }
            auto vertex = parse(line);
            vertices.push_back(vertex);
        }
        return vertices;
    }

    static Vector3f parse(std::string_view line)
    {
        auto count = StringCounter::countTokens(line);
        if (count != 3)
        {
            throw std::runtime_error("Invalid vertex, expected 3 tokens, got " + std::to_string(count));
        }
        return Parser::extractToken<Vector3f>(line);
    }
};

class FaceParser
{
public:
    static std::vector<Vector3ui> parseAll(FileStream &stream, const Dimensions &dimensions)
    {
        std::vector<Vector3ui> faces;
        auto count = dimensions.faceCount;
        faces.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            auto line = LineExtractor::nextLine(stream);
            if (line.empty())
            {
                auto message = "Not enough faces, expected " + std::to_string(count) + ", got " + std::to_string(i);
                throw std::runtime_error(message);
            }
            auto indices = parse(line, dimensions.vertexCount);
            faces.push_back(indices);
        }
        return faces;
    }

    static Vector3ui parse(std::string_view line, size_t vertexCount)
    {
        auto count = StringCounter::countTokens(line);
        if (count == 0)
        {
            throw std::runtime_error("Invalid face, expected at least one token, got " + std::to_string(count));
        }
        _extractSize(line);
        return _extractIndices(line, vertexCount);
    }

private:
    static size_t _extractSize(std::string_view &data)
    {
        auto size = Parser::extractToken<size_t>(data);
        if (size != 3)
        {
            throw std::runtime_error("Non triangular face with " + std::to_string(size) + " indices");
        }
        return size;
    }

    static Vector3ui _extractIndices(std::string_view &data, size_t vertexCount)
    {
        auto indices = Parser::extractToken<Vector3ui>(data);
        for (std::size_t i = 0; i < 3; ++i)
        {
            auto index = indices[i];
            if (index >= vertexCount)
            {
                auto message = "Invalid index: " + std::to_string(index) + " > " + std::to_string(vertexCount);
                throw std::runtime_error(message);
            }
        }
        return indices;
    }
};

class OffParser
{
public:
    static MeshBuffer parse(std::string_view data)
    {
        FileStream stream(data);
        try
        {
            return _parse(stream);
        }
        catch (const std::exception &e)
        {
            throw stream.error(e.what());
        }
    }

private:
    static MeshBuffer _parse(FileStream &stream)
    {
        auto mesh = MeshBuffer();
        HeaderParser::skipHeaderIfPresent(stream);
        auto line = stream.getLine();
        auto dimensions = DimensionsParser::parse(line);
        mesh.vertices = VertexParser::parseAll(stream, dimensions.vertexCount);
        mesh.indices = FaceParser::parseAll(stream, dimensions);
        return mesh;
    }
};

class MeshConverter
{
public:
    static TriangleMesh convert(const MeshBuffer &buffer)
    {
        TriangleMesh mesh;
        _reserve(buffer, mesh);
        for (const auto &triangle : buffer.indices)
        {
            _addVertex(buffer, mesh, triangle);
        }
        return mesh;
    }

private:
    static void _reserve(const MeshBuffer &buffer, TriangleMesh &mesh)
    {
        auto triangleCount = buffer.indices.size();
        auto size = 3 * triangleCount;
        mesh.vertices.reserve(size);
        mesh.indices.reserve(size);
    }

    static void _addVertex(const MeshBuffer &buffer, TriangleMesh &mesh, const Vector3ui &triangle)
    {
        auto offset = mesh.vertices.size();
        for (std::size_t i = 0; i < 3; ++i)
        {
            auto index = triangle[i];
            auto &vertex = buffer.vertices[index];
            mesh.vertices.push_back(vertex);
        }
        mesh.indices.emplace_back(offset, offset + 1, offset + 2);
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> OffMeshParser::getSupportedExtensions() const
{
    return {"off"};
}

TriangleMesh OffMeshParser::parse(std::string_view data) const
{
    auto buffer = OffParser::parse(data);
    return MeshConverter::convert(buffer);
}
} // namespace brayns
