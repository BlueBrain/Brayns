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

#include <utility>

#include <brayns/common/Log.h>

#include <brayns/utils/Convert.h>
#include <brayns/utils/string/FileStream.h>
#include <brayns/utils/string/ParsingException.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringStream.h>
#include <brayns/utils/string/StringTrimmer.h>

#include <brayns/common/TokenParser.h>

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

struct Line
{
    std::string_view key;
    std::string_view value;
};

class LineFormatter
{
public:
    static std::string_view removeCommentsAndTrim(std::string_view line)
    {
        auto stream = StringStream(line);
        line = stream.extractUntil('#');
        return StringTrimmer::trim(line);
    }
};

class LineParser
{
public:
    static Line parse(std::string_view data)
    {
        auto stream = StringStream(data);
        Line line;
        line.key = stream.extractToken();
        stream.extractSpaces();
        line.value = stream.extractAll();
        return line;
    }
};

class IndicesParser
{
public:
    static void parse(std::string_view value, MeshBuffer &mesh)
    {
        auto count = StringCounter::countTokens(value);
        if (count != 3)
        {
            throw std::runtime_error("Non-triangular face with " + std::to_string(count) + " vertices");
        }
        auto stream = StringStream(value);
        for (int i = 0; i < 3; ++i)
        {
            auto token = stream.extractToken();
            _parseToken(token, mesh);
        }
    }

private:
    static void _parseToken(std::string_view token, MeshBuffer &mesh)
    {
        auto count = StringCounter::count(token, '/');
        if (count > 2)
        {
            throw std::runtime_error("Invalid face element with " + std::to_string(count + 1) + "indices");
        }
        auto stream = StringStream(token);
        _parseIndex(stream, mesh.vertexIndices, mesh.vertices.size());
        _parseIndex(stream, mesh.textureIndices, mesh.textures.size());
        _parseIndex(stream, mesh.normalIndices, mesh.normals.size());
    }

    static void _parseIndex(StringStream &stream, std::vector<uint32_t> &indices, size_t elementCount)
    {
        auto data = stream.extractUntil('/');
        if (data.empty())
        {
            return;
        }
        auto index = Convert::fromString<uint32_t>(data);
        if (index < 1 || index > elementCount)
        {
            throw std::runtime_error("Invalid index " + std::to_string(index));
        }
        indices.push_back(index);
    }
};

class IndicesValidator
{
public:
    static void validate(const MeshBuffer &mesh)
    {
        auto vertexCount = mesh.vertexIndices.size();
        if (vertexCount == 0)
        {
            throw std::runtime_error("No vertex indices in face");
        }
        auto textureCount = mesh.textureIndices.size();
        if (!_equalOrEmpty(vertexCount, textureCount))
        {
            throw std::runtime_error("Face attribute count mismatch between vertices and textures");
        }
        auto normalCount = mesh.normalIndices.size();
        if (!_equalOrEmpty(vertexCount, normalCount))
        {
            throw std::runtime_error("Face attribute count mismatch between vertices and normals");
        }
        if (!_equalOrEmpty(normalCount, textureCount))
        {
            throw std::runtime_error("Face attribute count mismatch between normals and textures");
        }
    }

private:
    static bool _equalOrEmpty(size_t left, size_t right)
    {
        return left == right || left == 0 || right == 0;
    }
};

class ObjectParser
{
public:
    static MeshBuffer parse(std::string_view value)
    {
        MeshBuffer mesh;
        mesh.name = value;
        return mesh;
    }
};

class VertexParser
{
public:
    static Vector3f parse(std::string_view value)
    {
        auto count = StringCounter::countTokens(value);
        if (count != 3 || count != 4)
        {
            throw std::runtime_error("Invalid vertex, expected 3 or 4 tokens, got " + std::to_string(count));
        }
        return TokenParser::parse<Vector3f>(value);
    }
};

class TextureParser
{
public:
    static Vector2f parse(std::string_view value)
    {
        auto count = StringCounter::countTokens(value);
        if (count != 2)
        {
            throw std::runtime_error("Invalid texture, expected 2 tokens, got " + std::to_string(count));
        }
        return TokenParser::parse<Vector2f>(value);
    }
};

class NormalParser
{
public:
    static Vector3f parse(std::string_view value)
    {
        auto count = StringCounter::countTokens(value);
        if (count != 3)
        {
            throw std::runtime_error("Invalid normal, expected 3 tokens, got " + std::to_string(count));
        }
        return TokenParser::parse<Vector3f>(value);
    }
};

class FaceParser
{
public:
    static void parse(std::string_view value, MeshBuffer &mesh)
    {
        IndicesParser::parse(value, mesh);
        IndicesValidator::validate(mesh);
    }
};

class CurrentMesh
{
public:
    static MeshBuffer &get(std::vector<MeshBuffer> &meshes)
    {
        if (meshes.empty())
        {
            return meshes.emplace_back();
        }
        return meshes.back();
    }
};

class MeshLineParser
{
public:
    static bool parse(const Line &line, std::vector<MeshBuffer> &meshes)
    {
        auto &[key, value] = line;
        if (key == "o")
        {
            auto object = ObjectParser::parse(value);
            meshes.push_back(std::move(object));
            return true;
        }
        auto &mesh = CurrentMesh::get(meshes);
        if (key == "v")
        {
            auto vertex = VertexParser::parse(value);
            mesh.vertices.push_back(vertex);
            return true;
        }
        if (key == "vt")
        {
            auto texture = TextureParser::parse(value);
            mesh.textures.push_back(texture);
            return true;
        }
        if (key == "vn")
        {
            auto normal = NormalParser::parse(value);
            mesh.normals.push_back(normal);
            return true;
        }
        if (key == "f")
        {
            FaceParser::parse(value, mesh);
            return true;
        }
        return false;
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
            _checkCompatibility(first, meshes[i], i);
        }
    }

private:
    static void _checkCompatibility(const MeshBuffer &left, const MeshBuffer &right, size_t index)
    {
        if (!_checkCompatibility(left.normalIndices, right.normalIndices))
        {
            throw std::runtime_error("Mesh " + std::to_string(index) + " normals incompatible with first one");
        }
        if (!_checkCompatibility(left.textureIndices, right.textureIndices))
        {
            throw std::runtime_error("Mesh " + std::to_string(index) + " textures incompatible with first one");
        }
    }

    static bool _checkCompatibility(const std::vector<uint32_t> &left, const std::vector<uint32_t> &right)
    {
        if (left.size() == right.size())
        {
            return true;
        }
        if (left.empty() && right.empty())
        {
            return true;
        }
        return false;
    }
};

class ObjParser
{
public:
    static std::vector<MeshBuffer> parse(std::string_view data)
    {
        FileStream stream(data);
        try
        {
            return _parse(stream);
        }
        catch (const std::exception &e)
        {
            throw ParsingException(e.what(), stream);
        }
    }

private:
    static std::vector<MeshBuffer> _parse(FileStream &stream)
    {
        std::vector<MeshBuffer> meshes;
        while (stream.nextLine())
        {
            auto data = stream.getLine();
            data = LineFormatter::removeCommentsAndTrim(data);
            if (data.empty())
            {
                Log::debug("Skip empty line {} '{}'.", stream.getLineNumber(), stream.getLine());
                continue;
            }
            auto line = LineParser::parse(data);
            if (!MeshLineParser::parse(line, meshes))
            {
                Log::debug("Skip unknown line {} '{}'", stream.getLineNumber(), stream.getLine());
            }
        }
        MeshValidator::validate(meshes);
        return meshes;
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
