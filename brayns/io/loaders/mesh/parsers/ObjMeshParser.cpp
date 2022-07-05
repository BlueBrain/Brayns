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

#include <brayns/utils/Convert.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringInfo.h>
#include <brayns/utils/string/StringStream.h>
#include <brayns/utils/string/StringTrimmer.h>

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
    size_t lineNumber = 0;
    std::string_view line;
};

struct Line
{
    std::string_view key;
    std::string_view value;
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

class LineParser
{
public:
    static Line parse(std::string_view data)
    {
        auto stream = StringStream(data);
        Line line;
        line.key = stream.extractToken();
        line.value = stream.extractToken();
        return line;
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

class VectorParser
{
public:
    template<glm::length_t S, typename T>
    static void parse(std::string_view data, glm::vec<S, T> &value)
    {
        if (StringCounter::countTokens(data) != S)
        {
            auto size = std::to_string(S);
            throw std::runtime_error("Expected " + size + " numbers for a vector" + size);
        }
        auto stream = StringStream(data);
        for (glm::length_t i = 0; i < S; ++i)
        {
            auto token = stream.extractToken();
            Convert::fromString(token, value[i]);
        }
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
            throw std::runtime_error("Invalid index count of " + std::to_string(count) + " in a vertex");
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

class NewObjectParser
{
public:
    static bool canParse(std::string_view key)
    {
        return key == "o";
    }

    static void parse(std::string_view value, std::vector<MeshBuffer> &meshes)
    {
        auto &mesh = meshes.emplace_back();
        mesh.name = value;
    }
};

class VertexParser
{
public:
    static bool canParse(std::string_view key)
    {
        return key == "v";
    }

    static void parse(std::string_view value, std::vector<MeshBuffer> &meshes)
    {
        auto &mesh = CurrentMesh::get(meshes);
        auto &vertex = mesh.vertices.emplace_back();
        VectorParser::parse(value, vertex);
    }
};

class TextureParser
{
public:
    static bool canParse(std::string_view key)
    {
        return key == "vt";
    }

    static void parse(std::string_view value, std::vector<MeshBuffer> &meshes)
    {
        auto &mesh = CurrentMesh::get(meshes);
        auto &texture = mesh.textures.emplace_back();
        VectorParser::parse(value, texture);
    }
};

class NormalParser
{
public:
    static bool canParse(std::string_view key)
    {
        return key == "vn";
    }

    static void parse(std::string_view value, std::vector<MeshBuffer> &meshes)
    {
        auto &mesh = CurrentMesh::get(meshes);
        auto &normal = mesh.normals.emplace_back();
        VectorParser::parse(value, normal);
    }
};

class FaceParser
{
public:
    static bool canParse(std::string_view key)
    {
        return key == "f";
    }

    static void parse(std::string_view value, std::vector<MeshBuffer> &meshes)
    {
        auto &mesh = CurrentMesh::get(meshes);
        IndicesParser::parse(value, mesh);
        IndicesValidator::validate(mesh);
    }
};

class LineExtractor
{
public:
    static bool extract(const Line &line, std::vector<MeshBuffer> &meshes)
    {
        bool result = false;
        result |= _tryParseWith<NewObjectParser>(line, meshes);
        result |= _tryParseWith<VertexParser>(line, meshes);
        result |= _tryParseWith<TextureParser>(line, meshes);
        result |= _tryParseWith<NormalParser>(line, meshes);
        result |= _tryParseWith<FaceParser>(line, meshes);
        return result;
    }

private:
    template<typename T>
    static bool _tryParseWith(const Line &line, std::vector<MeshBuffer> &meshes)
    {
        if (!T::canParse(line.key))
        {
            return false;
        }
        T::parse(line.value, meshes);
        return true;
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

class ObjParser
{
public:
    static std::vector<MeshBuffer> parse(std::string_view data)
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
    static std::vector<MeshBuffer> _parse(Context &context, std::string_view data)
    {
        std::vector<MeshBuffer> meshes;
        auto stream = StringStream(data);
        while (LineReader::read(context, stream))
        {
            auto data = LineFormatter::format(context.line);
            if (data.empty())
            {
                Log::debug("Skip empty line {} '{}'.", context.lineNumber, context.line);
                continue;
            }
            auto line = LineParser::parse(data);
            if (!LineExtractor::extract(line, meshes))
            {
                Log::debug("Skip unknown line {} '{}'", context.lineNumber, context.line);
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
