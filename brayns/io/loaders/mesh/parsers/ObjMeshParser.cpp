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
    std::string_view key;
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

    static MeshBuffer &next(Context &context)
    {
        auto &meshes = context.meshes;
        return meshes.emplace_back();
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
        context.key = StringHelper::extractToken(line);
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
        return {StringHelper::extract<float>(str), StringHelper::extract<float>(str)};
    }

    static Vector3f parseVector3(std::string_view str)
    {
        if (StringHelper::countTokens(str) != 3)
        {
            throw std::runtime_error("Expected 3 numbers for a vector3");
        }
        return {
            StringHelper::extract<float>(str),
            StringHelper::extract<float>(str),
            StringHelper::extract<float>(str)};
    }
};

class NewObjectParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "o";
    }

    static void parse(Context &context)
    {
        auto &mesh = CurrentMesh::next(context);
        mesh.name = context.value;
    }
};

class VertexParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "v";
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
        return context.key == "vt";
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
        return context.key == "vn";
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
        if (vertexCount == 0)
        {
            throw std::runtime_error("No vertex indices in face");
        }
        auto textureCount = mesh.textureIndices.size();
        auto normalCount = mesh.normalIndices.size();
        return _checkSameOrEmpty(vertexCount, textureCount) && _checkSameOrEmpty(vertexCount, normalCount)
            && _checkSameOrEmpty(textureCount, normalCount);
    }

    static bool _checkSameOrEmpty(size_t left, size_t right)
    {
        return left == 0 || right == 0 || left == right;
    }
};

class FaceParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "f";
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
            throw std::runtime_error("Non-triangular face with " + std::to_string(count) + " vertices");
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
            throw std::runtime_error("Invalid index count of " + std::to_string(count) + " in a vertex");
        }
        _extractIndex(indices, mesh.vertexIndices, mesh.vertices.size());
        _extractIndex(indices, mesh.textureIndices, mesh.textures.size());
        _extractIndex(indices, mesh.normalIndices, mesh.normals.size());
    }

    static void _extractIndex(std::string_view &tokens, std::vector<uint32_t> &indices, size_t size)
    {
        auto token = StringHelper::extract(tokens, "/");
        if (token.empty())
        {
            return;
        }
        auto index = StringHelper::extract<uint32_t>(token);
        if (index < 1 || index > size)
        {
            throw std::runtime_error("Invalid index " + std::to_string(index));
        }
        indices.push_back(index);
    }
};

class MeshExtractor
{
public:
    static bool tryExtractMeshData(Context &context)
    {
        return _tryParseWith<NewObjectParser>(context) || _tryParseWith<VertexParser>(context)
            || _tryParseWith<TextureParser>(context) || _tryParseWith<NormalParser>(context)
            || _tryParseWith<FaceParser>(context);
    }

private:
    template<typename T>
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
    static std::string format(const Context &context, const std::string &message)
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

class MeshValidator
{
public:
    static void validate(const std::vector<MeshBuffer> &meshes)
    {
        if (meshes.empty())
        {
            throw std::runtime_error("No meshes found");
        }
        auto &first = meshes.front();
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            if (!_checkCompatibility(first, meshes[i]))
            {
                throw std::runtime_error("Mesh number" + std::to_string(i) + " is not compatible with the first one");
            }
        }
    }

private:
    static bool _checkCompatibility(const MeshBuffer &left, const MeshBuffer &right)
    {
        auto leftTextureCount = left.textureIndices.size();
        auto rightTextureCount = right.textureIndices.size();
        auto leftNormalCount = left.normalIndices.size();
        auto rightNormalCount = right.normalIndices.size();
        return _bothOrNoneAreEmpty(leftTextureCount, rightTextureCount)
            && _bothOrNoneAreEmpty(leftNormalCount, rightNormalCount);
    }

    static bool _bothOrNoneAreEmpty(size_t left, size_t right)
    {
        return (left == 0 && right == 0) || (left != 0 && right != 0);
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
        MeshValidator::validate(context.meshes);
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
    static TriangleMesh convert(const std::vector<MeshBuffer> &buffers)
    {
        TriangleMesh mesh;
        _reserve(buffers, mesh);
        _fill(buffers, mesh);
        return mesh;
    }

private:
    static void _reserve(const std::vector<MeshBuffer> &buffers, TriangleMesh &mesh)
    {
        size_t vertexCount = 0;
        size_t normalCount = 0;
        size_t textureCount = 0;
        size_t indexCount = 0;
        for (const auto &buffer : buffers)
        {
            vertexCount += buffer.vertices.size();
            normalCount += buffer.normals.size();
            textureCount += buffer.textures.size();
            indexCount += buffer.vertexIndices.size();
        }
        mesh.vertices.reserve(vertexCount);
        mesh.normals.reserve(normalCount);
        mesh.uvs.reserve(textureCount);
        mesh.indices.reserve(indexCount);
    }

    static void _fill(const std::vector<MeshBuffer> &buffers, TriangleMesh &mesh)
    {
        size_t offset = 0;
        for (const auto &buffer : buffers)
        {
            auto indexCount = buffer.vertexIndices.size();
            for (size_t index = 0; index < indexCount; index += 3)
            {
                _addVertex(buffer, mesh, index);
                _addVertex(buffer, mesh, index + 1);
                _addVertex(buffer, mesh, index + 2);
                _addTriangleIndices(mesh, offset + index);
            }
            offset += indexCount;
        }
    }

    static void _addVertex(const MeshBuffer &buffer, TriangleMesh &mesh, size_t index)
    {
        auto vertexIndex = buffer.vertexIndices[index] - 1;
        auto &vertex = buffer.vertices[vertexIndex];
        mesh.vertices.push_back(vertex);
        if (!buffer.textureIndices.empty())
        {
            auto textureIndex = buffer.textureIndices[index] - 1;
            auto &texture = buffer.textures[textureIndex];
            mesh.uvs.push_back(texture);
        }
        if (!buffer.normalIndices.empty())
        {
            auto normalIndex = buffer.normalIndices[index] - 1;
            auto &normal = buffer.normals[normalIndex];
            mesh.normals.push_back(normal);
        }
    }

    static void _addTriangleIndices(TriangleMesh &mesh, size_t baseIndex)
    {
        mesh.indices.emplace_back(baseIndex, baseIndex + 1, baseIndex + 2);
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> ObjMeshParser::getSupportedExtensions() const
{
    return {"obj"};
}

TriangleMesh ObjMeshParser::parse(std::string_view data) const
{
    auto buffers = ObjParser::parse(data);
    return MeshConverter::convert(buffers);
}
} // namespace brayns
