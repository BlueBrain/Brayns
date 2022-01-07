/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "helpers/BinaryHelper.h"
#include "helpers/StreamHelper.h"
#include "helpers/StringHelper.h"

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
    std::string_view data;
    size_t lineNumber = 0;
    std::string_view line;
    size_t vertexCount = 0;
    size_t faceCount = 0;
    size_t edgeCount = 0;
    MeshBuffer mesh;

    Context(std::string_view source)
        : data(source)
    {
    }
};

class LineExtractor
{
public:
    static bool nextNonEmptyLine(Context &context)
    {
        while (true)
        {
            auto &data = context.data;
            auto &line = context.line;
            if (!StreamHelper::getLine(data, line))
            {
                return false;
            }
            ++context.lineNumber;
            line = StringHelper::extract(line, "#");
            if (!StringHelper::isSpace(line))
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
    static void skipHeader(Context &context)
    {
        while (true)
        {
            if (!LineExtractor::nextNonEmptyLine(context))
            {
                throw std::runtime_error("Empty file");
            }
            auto line = context.line;
            auto token = StringHelper::extractToken(line);
            if (token != "OFF")
            {
                return;
            }
        }
        throw std::runtime_error("Internal error");
    }
};

class DimensionsParser
{
public:
    static void parse(Context &context)
    {
        auto line = context.line;
        if (StringHelper::countTokens(line) != 3)
        {
            throw std::runtime_error("Missing dimensions");
        }
        context.vertexCount = StringHelper::extract<size_t>(line);
        context.faceCount = StringHelper::extract<size_t>(line);
        context.edgeCount = StringHelper::extract<size_t>(line);
    }
};

class VertexParser
{
public:
    static void parseAll(Context &context)
    {
        auto count = context.vertexCount;
        for (size_t i = 0; i < count; ++i)
        {
            LineExtractor::nextNonEmptyLine(context);
            _parse(context);
        }
    }

private:
    static void _parse(Context &context)
    {
        auto line = context.line;
        if (StringHelper::countTokens(line) != 3)
        {
            throw std::runtime_error("Expected 3 values for a vertex");
        }
        auto &mesh = context.mesh;
        auto &vertices = mesh.vertices;
        auto &vertex = vertices.emplace_back();
        vertex.x = StringHelper::extract<float>(line);
        vertex.y = StringHelper::extract<float>(line);
        vertex.z = StringHelper::extract<float>(line);
    }
};

class FaceParser
{
public:
    static void parseAll(Context &context)
    {
        auto count = context.faceCount;
        for (size_t i = 0; i < count; ++i)
        {
            LineExtractor::nextNonEmptyLine(context);
            _parse(context);
        }
    }

private:
    static void _parse(Context &context)
    {
        auto line = context.line;
        if (StringHelper::countTokens(line) != 4)
        {
            throw std::runtime_error("Expected 4 values for a face");
        }
        auto size = StringHelper::extract<size_t>(line);
        if (size != 3)
        {
            throw std::runtime_error("Non triangular face");
        }
        _extractIndices(context, line);
    }

    static void _extractIndices(Context &context, std::string_view &line)
    {
        auto vertexCount = context.vertexCount;
        auto &mesh = context.mesh;
        auto &indices = mesh.indices;
        auto &triangle = indices.emplace_back();
        triangle[0] = _extractIndex(line, vertexCount);
        triangle[1] = _extractIndex(line, vertexCount);
        triangle[2] = _extractIndex(line, vertexCount);
    }

    static uint32_t _extractIndex(std::string_view &line, size_t vertexCount)
    {
        auto index = StringHelper::extract<uint32_t>(line);
        if (index > vertexCount)
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
        Context context(data);
        try
        {
            _parse(context);
            return context.mesh;
        }
        catch (const std::exception &e)
        {
            auto message = ErrorMessage::format(context, e.what());
            throw std::runtime_error(message);
        }
    }

private:
    static void _parse(Context &context)
    {
        HeaderParser::skipHeader(context);
        DimensionsParser::parse(context);
        VertexParser::parseAll(context);
        FaceParser::parseAll(context);
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

    static void _addVertex(const MeshBuffer &buffer, TriangleMesh &mesh,
                           const Vector3ui &triangle)
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
