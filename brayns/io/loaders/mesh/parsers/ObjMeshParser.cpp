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

#include "ObjMeshParser.h"

#include <sstream>
#include <utility>

#include <brayns/common/Log.h>

#include "helpers/StreamHelper.h"
#include "helpers/StringHelper.h"

namespace
{
using namespace brayns;

struct MeshBuffer
{
    std::string name;
    std::vector<Vector3f> vertices;
    std::vector<Vector2f> textures;
    std::vector<Vector3f> normals;
    std::vector<uint32_t> vertexIndices;
    std::vector<uint32_t> textureIndices;
    std::vector<uint32_t> normalIndices;
};

struct Context
{
    std::string_view data;
    size_t lineNumber = 0;
    std::string_view line;
    std::string_view header;
    std::string_view value;
    std::vector<MeshBuffer> meshes;

    Context(std::string_view source)
        : data(source)
    {
    }
};

class CurrentMesh
{
public:
    static MeshBuffer &get(Context &context)
    {
        auto &meshes = context.meshes;
        if (meshes.empty())
        {
            return next(context);
        }
        return meshes.back();
    }

    static MeshBuffer &next(Context &context, std::string_view name = "")
    {
        auto &meshes = context.meshes;
        auto &mesh = meshes.emplace_back();
        mesh.name = name;
        Log::debug("New mesh found: {}.", mesh.name);
        return mesh;
    }
};

class LineParser
{
public:
    static bool parseHeaderAndValue(Context &context)
    {
        std::string_view line = context.line;
        if (_isEmptyLineOrComment(line))
        {
            return false;
        }
        _extractHeaderAndValue(context, line);
        return true;
    }

private:
    static bool _isEmptyLineOrComment(std::string_view line)
    {
        line = StringHelper::trimLeft(line);
        return line.empty() || line[0] == '#';
    }

    static void _extractHeaderAndValue(Context &context, std::string_view line)
    {
        context.header = StringHelper::extractToken(line);
        context.value = line;
    }
};

class ValueParser
{
public:
    static Vector2f parseVector2(std::string_view str)
    {
        if (StringHelper::countTokens(str) != 2)
        {
            throw std::runtime_error("Expected 2 numbers for a vector2");
        }
        return {StringHelper::extract<float>(str),
                StringHelper::extract<float>(str)};
    }

    static Vector3f parseVector3(std::string_view str)
    {
        if (StringHelper::countTokens(str) != 3)
        {
            throw std::runtime_error("Expected 3 numbers for a vector3");
        }
        return {StringHelper::extract<float>(str),
                StringHelper::extract<float>(str),
                StringHelper::extract<float>(str)};
    }
};

class NewObjectParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.header == "o";
    }

    static void parse(Context &context)
    {
        auto &name = context.value;
        CurrentMesh::next(context, name);
    }
};

class VertexParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.header == "v";
    }

    static void parse(Context &context)
    {
        auto &value = context.value;
        auto vertex = ValueParser::parseVector3(value);
        auto &mesh = CurrentMesh::get(context);
        mesh.vertices.push_back(vertex);
    }
};

class TextureParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.header == "vt";
    }

    static void parse(Context &context)
    {
        auto &value = context.value;
        auto texture = ValueParser::parseVector2(value);
        auto &mesh = CurrentMesh::get(context);
        mesh.textures.push_back(texture);
    }
};

class NormalParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.header == "vn";
    }

    static void parse(Context &context)
    {
        auto &value = context.value;
        auto normal = ValueParser::parseVector3(value);
        auto &mesh = CurrentMesh::get(context);
        mesh.normals.push_back(normal);
    }
};

class FaceValidator
{
public:
    static void validate(const MeshBuffer &mesh)
    {
        if (!_checkAttributeCount(mesh))
        {
            throw std::runtime_error("Face attribute count mismatch");
        }
    }

private:
    static bool _checkAttributeCount(const MeshBuffer &mesh)
    {
        auto vertexCount = mesh.vertexIndices.size();
        auto textureCount = mesh.textureIndices.size();
        auto normalCount = mesh.normalIndices.size();
        return _checkAttributeCount(vertexCount, textureCount) &&
               _checkAttributeCount(vertexCount, normalCount) &&
               _checkAttributeCount(textureCount, normalCount);
    }

    static bool _checkAttributeCount(size_t left, size_t right)
    {
        return left == 0 || right == 0 || left == right;
    }
};

class FaceParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.header == "f";
    }

    static void parse(Context &context)
    {
        auto &value = context.value;
        auto &mesh = CurrentMesh::get(context);
        _extractVertices(value, mesh);
    }

private:
    static void _extractVertices(std::string_view value, MeshBuffer &mesh)
    {
        auto count = StringHelper::countTokens(value);
        if (count != 3)
        {
            throw std::runtime_error("Non-triangular face with " +
                                     std::to_string(count) + " vertices");
        }
        _extractIndices(value, mesh);
        _extractIndices(value, mesh);
        _extractIndices(value, mesh);
        FaceValidator::validate(mesh);
    }

    static void _extractIndices(std::string_view &value, MeshBuffer &mesh)
    {
        auto indices = StringHelper::extractToken(value);
        auto count = StringHelper::count(indices, "/");
        if (count > 2)
        {
            throw std::runtime_error("Invalid index count of " +
                                     std::to_string(count) + " in a vertex");
        }
        _extractIndex(indices, mesh.vertexIndices);
        _extractIndex(indices, mesh.textureIndices);
        _extractIndex(indices, mesh.normalIndices);
    }

    static void _extractIndex(std::string_view &tokens,
                              std::vector<uint32_t> &vertices)
    {
        auto token = StringHelper::extract(tokens, "/");
        if (token.empty())
        {
            return;
        }
        auto index = StringHelper::extract<uint32_t>(token);
        vertices.push_back(index);
    }
};

class MeshExtractor
{
public:
    static bool tryExtractMeshData(Context &context)
    {
        return _tryParseWith<NewObjectParser>(context) ||
               _tryParseWith<VertexParser>(context) ||
               _tryParseWith<TextureParser>(context) ||
               _tryParseWith<NormalParser>(context) ||
               _tryParseWith<FaceParser>(context);
    }

private:
    template <typename T>
    static bool _tryParseWith(Context &context)
    {
        if (!T::canParse(context))
        {
            return false;
        }
        T::parse(context);
        return true;
    }
};

class ErrorMessage
{
public:
    static std::string format(const Context &context,
                              const std::string &message)
    {
        std::ostringstream stream;
        stream << "Parsing error at line ";
        stream << context.lineNumber;
        stream << ": '";
        stream << message;
        stream << "'. Line content: '";
        stream << context.line;
        stream << "'";
        return stream.str();
    }
};

class ObjParser
{
public:
    static std::vector<MeshBuffer> parse(std::string_view data)
    {
        Context context(data);
        _tryParse(context);
        return context.meshes;
    }

private:
    static void _tryParse(Context &context)
    {
        try
        {
            _parse(context);
        }
        catch (const std::exception &e)
        {
            auto message = ErrorMessage::format(context, e.what());
            throw std::runtime_error(message);
        }
    }

    static void _parse(Context &context)
    {
        while (_extractLine(context))
        {
            _parseLine(context);
        }
    }

    static bool _extractLine(Context &context)
    {
        if (!StreamHelper::getLine(context.data, context.line))
        {
            return false;
        }
        ++context.lineNumber;
        return true;
    }

    static void _parseLine(Context &context)
    {
        if (!LineParser::parseHeaderAndValue(context))
        {
            return;
        }
        if (!MeshExtractor::tryExtractMeshData(context))
        {
            Log::debug("Skip line {} '{}'.", context.lineNumber, context.line);
        }
    }
};

class MeshConverter
{
public:
    static std::vector<TriangleMesh> convert(
        const std::vector<MeshBuffer> &buffers)
    {
        std::vector<TriangleMesh> meshes;
        meshes.reserve(buffers.size());
        for (const auto &buffer : buffers)
        {
            auto mesh = convert(buffer);
            meshes.push_back(std::move(mesh));
        }
        return meshes;
    }

    static TriangleMesh convert(const MeshBuffer &buffer)
    {
        TriangleMesh mesh;
        _reserve(mesh, buffer);
        _fill(mesh, buffer);
        return mesh;
    }

private:
    static void _reserve(TriangleMesh &mesh, const MeshBuffer &buffer)
    {
        mesh.vertices.reserve(buffer.vertexIndices.size());
        mesh.textureCoordinates.reserve(buffer.textureIndices.size());
        mesh.normals.reserve(buffer.normalIndices.size());
    }

    static void _fill(TriangleMesh &mesh, const MeshBuffer &buffer)
    {
        for (size_t i = 0; i < buffer.vertexIndices.size(); i += 3)
        {
            _addVertex(mesh, buffer, i);
            _addVertex(mesh, buffer, i + 1);
            _addVertex(mesh, buffer, i + 2);
            auto index = uint32_t(i);
            mesh.indices.emplace_back(index, index + 1, index + 2);
        }
    }

    static void _addVertex(TriangleMesh &mesh, const MeshBuffer &buffer,
                           size_t index)
    {
        auto vertexIndex = buffer.vertexIndices[index];
        auto &vertex = buffer.vertices[vertexIndex - 1];
        auto textureIndex = buffer.textureIndices[index];
        auto &texture = buffer.textures[textureIndex - 1];
        auto normalIndex = buffer.normalIndices[index];
        auto &normal = buffer.normals[normalIndex - 1];
        mesh.vertices.push_back(vertex);
        mesh.textureCoordinates.push_back(texture);
        mesh.normals.push_back(normal);
    }
};
} // namespace

namespace brayns
{
std::string ObjMeshParser::getFormat() const
{
    return "obj";
}

std::vector<TriangleMesh> ObjMeshParser::parse(std::string_view data) const
{
    auto buffers = ObjParser::parse(data);
    return MeshConverter::convert(buffers);
}
} // namespace brayns
