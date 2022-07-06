/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/utils/Convert.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringStream.h>
#include <brayns/utils/string/StringTrimmer.h>

namespace
{
using namespace brayns;

struct MeshBuffer
{
    std::vector<Vector3f> vertices;
    std::vector<Vector3ui> indices;
};

struct Context
{
    size_t lineNumber = 0;
    std::string_view line;
};

struct Dimensions
{
    size_t vertexCount = 0;
    size_t faceCount = 0;
    size_t edgeCount = 0;
};

class LineReader
{
public:
    static bool read(Context &context, StringStream &stream)
    {
        if (stream.isEmpty())
        {
            return false;
        }
        context.line = stream.extractLine();
        ++context.lineNumber;
        return true;
    }
};

class LineFormatter
{
public:
    static std::string_view format(std::string_view data)
    {
        auto stream = StringStream(data);
        auto result = stream.extractUntil('#');
        return StringTrimmer::trim(result);
    }
};

class LineExtractor
{
public:
    static bool nextNonEmptyLine(Context &context, StringStream &stream)
    {
        while (true)
        {
            if (!LineReader::read(context, stream))
            {
                return false;
            }
            auto line = LineFormatter::format(context.line);
            if (!line.empty())
            {
                return true;
            }
        }
        throw std::runtime_error("Internal error");
    }
};

class HeaderParser
{
public:
    static void skipHeaderIfPresent(Context &context, StringStream &stream)
    {
        if (!LineExtractor::nextNonEmptyLine(context, stream))
        {
            throw std::runtime_error("Empty file");
        }
        if (context.line != "OFF")
        {
            return;
        }
        if (!LineExtractor::nextNonEmptyLine(context, stream))
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
            throw std::runtime_error("Expected 3 tokens for dimensions, got " + std::to_string(count));
        }
        Dimensions dimensions;
        auto stream = StringStream(line);
        dimensions.vertexCount = _extractDimension(stream);
        dimensions.faceCount = _extractDimension(stream);
        dimensions.edgeCount = _extractDimension(stream);
        return dimensions;
    }

private:
    static size_t _extractDimension(StringStream &stream)
    {
        auto token = stream.extractToken();
        return Convert::fromString<size_t>(token);
    }
};

class VertexParser
{
public:
    static std::vector<Vector3f> parseAll(Context &context, StringStream &stream, size_t count)
    {
        std::vector<Vector3f> vertices;
        vertices.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            if (!LineExtractor::nextNonEmptyLine(context, stream))
            {
                throw std::runtime_error("Expected " + std::to_string(count) + " vertices");
            }
            auto vertex = parse(context.line);
            vertices.push_back(vertex);
        }
        return vertices;
    }

    static Vector3f parse(std::string_view line)
    {
        auto count = StringCounter::countTokens(line);
        if (count != 3)
        {
            throw std::runtime_error("Expected 3 tokens for a vertex, got " + std::to_string(count));
        }
        Vector3f vertex;
        auto stream = StringStream(line);
        vertex.x = _extractFloat(stream);
        vertex.y = _extractFloat(stream);
        vertex.z = _extractFloat(stream);
        return vertex;
    }

private:
    static float _extractFloat(StringStream &stream)
    {
        auto token = stream.extractToken();
        return Convert::fromString<float>(token);
    }
};

class FaceParser
{
public:
    static std::vector<Vector3ui> parseAll(Context &context, StringStream &stream, const Dimensions &dimensions)
    {
        std::vector<Vector3ui> faces;
        auto count = dimensions.faceCount;
        faces.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            if (!LineExtractor::nextNonEmptyLine(context, stream))
            {
                throw std::runtime_error("Expected " + std::to_string(count) + " faces");
            }
            auto indices = parse(context.line, dimensions.vertexCount);
            faces.push_back(indices);
        }
        return faces;
    }

    static Vector3ui parse(std::string_view line, size_t vertexCount)
    {
        auto count = StringCounter::countTokens(line);
        if (count != 4)
        {
            throw std::runtime_error("Expected 4 tokens for a face, got " + std::to_string(count));
        }
        auto stream = StringStream(line);
        _extractSize(stream);
        return _extractIndices(stream, vertexCount);
    }

private:
    static size_t _extractSize(StringStream &stream)
    {
        auto token = stream.extractToken();
        auto size = Convert::fromString<size_t>(token);
        if (size != 3)
        {
            throw std::runtime_error("Non triangular face with " + std::to_string(size) + " indices");
        }
        return size;
    }

    static Vector3f _extractIndices(StringStream &stream, size_t vertexCount)
    {
        Vector3ui indices;
        indices[0] = _extractIndex(stream, vertexCount);
        indices[1] = _extractIndex(stream, vertexCount);
        indices[2] = _extractIndex(stream, vertexCount);
        return indices;
    }

    static uint32_t _extractIndex(StringStream &stream, size_t vertexCount)
    {
        auto token = stream.extractToken();
        auto index = Convert::fromString<uint32_t>(token);
        if (index >= vertexCount)
        {
            throw std::runtime_error("Invalid index " + std::to_string(index));
        }
        return index;
    }
};

class ErrorMessage
{
public:
    static std::string format(const Context &context, const char *message)
    {
        std::ostringstream stream;
        stream << "Parsing error at line " << context.lineNumber;
        stream << ": '" << message << "'";
        stream << ". Line content: '" << context.line << "'";
        return stream.str();
    }
};

class OffParser
{
public:
    static MeshBuffer parse(std::string_view data)
    {
        Context context;
        try
        {
            return _parse(context, data);
        }
        catch (const std::exception &e)
        {
            auto message = ErrorMessage::format(context, e.what());
            throw std::runtime_error(message);
        }
    }

private:
    static MeshBuffer _parse(Context &context, std::string_view data)
    {
        auto mesh = MeshBuffer();
        auto stream = StringStream(data);
        HeaderParser::skipHeaderIfPresent(context, stream);
        auto dimensions = DimensionsParser::parse(context.line);
        VertexParser::parseAll(context, stream, dimensions.vertexCount);
        FaceParser::parseAll(context, stream, dimensions);
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
        for (auto index : triangle)
        {
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
