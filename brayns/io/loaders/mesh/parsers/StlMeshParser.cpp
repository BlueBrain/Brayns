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

#include "StlMeshParser.h"

#include <array>

#include <brayns/common/GlmParsers.h>

#include <brayns/utils/parsing/Endian.h>
#include <brayns/utils/parsing/FileStream.h>
#include <brayns/utils/parsing/Parse.h>
#include <brayns/utils/parsing/ParsingException.h>
#include <brayns/utils/parsing/StringCounter.h>
#include <brayns/utils/parsing/StringStream.h>
#include <brayns/utils/parsing/StringTrimmer.h>

namespace
{
using namespace brayns;

struct Facet
{
    Vector3f normal;
    std::array<Vector3f, 3> vertices;
};

struct Solid
{
    std::string name;
    std::vector<Facet> facets;
};

class Format
{
public:
    static bool isAscii(std::string_view data)
    {
        return data.substr(0, 5) == "solid";
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
            line = StringTrimmer::trim(line);
            if (!line.empty())
            {
                return line;
            }
        }
    }
};

class FixedStringExtractor
{
public:
    static void checkLine(std::string_view line, std::string_view expected)
    {
        if (line != expected)
        {
            auto message = "Invalid line, expected '" + std::string(expected) + "', got '" + std::string(line) + "'";
            throw std::runtime_error(message);
        }
    }

    static void extractToken(StringStream &stream, std::string_view expected)
    {
        auto token = stream.extractToken();
        if (token != expected)
        {
            auto message = "Invalid token, expected '" + std::string(expected) + "', got '" + std::string(token) + "'";
            throw std::runtime_error(message);
        }
    }
};

class AsciiNormalParser
{
public:
    static Vector3f parse(std::string_view line)
    {
        auto count = StringCounter::countTokens(line);
        if (count != 5)
        {
            throw std::runtime_error("Invalid normal, expected 5 tokens, got " + std::to_string(count));
        }
        auto stream = StringStream(line);
        FixedStringExtractor::extractToken(stream, "facet");
        FixedStringExtractor::extractToken(stream, "normal");
        return Parse::fromTokens<Vector3f>(stream);
    }
};

class AsciiVertexParser
{
public:
    static Vector3f parse(std::string_view line)
    {
        auto count = StringCounter::countTokens(line);
        if (count != 4)
        {
            throw std::runtime_error("Invalid vertex, expected 4 tokens, got " + std::to_string(count));
        }
        auto stream = StringStream(line);
        auto vertex = stream.extractToken();
        if (vertex != "vertex")
        {
            throw std::runtime_error("Invalid vertex, expected 'vertex', got " + std::string(vertex));
        }
        return Parse::fromTokens<Vector3f>(stream);
    }
};

class AsciiFacetParser
{
public:
    static Facet parse(FileStream &stream)
    {
        Facet facet;
        facet.normal = _parseNormal(stream);
        _skipOuterLoop(stream);
        facet.vertices = _parseVertices(stream);
        _skipEndLoop(stream);
        _skipEndFacet(stream);
        return facet;
    }

private:
    static std::string_view _nextLine(FileStream &stream)
    {
        auto line = LineExtractor::nextLine(stream);
        if (line.empty())
        {
            throw std::runtime_error("Unterminated facet");
        }
        return line;
    }

    static Vector3f _parseNormal(FileStream &stream)
    {
        auto line = _nextLine(stream);
        return AsciiNormalParser::parse(line);
    }

    static void _skipOuterLoop(FileStream &stream)
    {
        auto line = _nextLine(stream);
        FixedStringExtractor::checkLine(line, "outer loop");
    }

    static std::array<Vector3f, 3> _parseVertices(FileStream &stream)
    {
        std::array<Vector3f, 3> vertices;
        for (auto &vertex : vertices)
        {
            auto line = _nextLine(stream);
            vertex = AsciiVertexParser::parse(line);
        }
        return vertices;
    }

    static void _skipEndLoop(FileStream &stream)
    {
        auto line = _nextLine(stream);
        FixedStringExtractor::checkLine(line, "endloop");
    }

    static void _skipEndFacet(FileStream &stream)
    {
        auto line = _nextLine(stream);
        FixedStringExtractor::checkLine(line, "endfacet");
    }
};

class AsciiSolidParser
{
public:
    static Solid parse(std::string_view data)
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
    static std::string_view _nextLine(FileStream &stream)
    {
        auto line = LineExtractor::nextLine(stream);
        if (line.empty())
        {
            throw std::runtime_error("Unterminated solid");
        }
        return line;
    }

    static Solid _parse(FileStream &stream)
    {
        auto solid = _beginSolid(stream);
        while (true)
        {
            auto line = _nextLine(stream);
            if (line == "endsolid")
            {
                return solid;
            }
        }
    }

    static Solid _beginSolid(FileStream &stream)
    {
        auto line = _nextLine(stream);
        auto header = StringStream(line);
        FixedStringExtractor::extractToken(header, "solid");
        header.extractSpaces();
        Solid solid;
        solid.name = header.extractAll();
        return solid;
    }
};

class BinaryHeaderParser
{
public:
    static size_t parse(StringStream &stream)
    {
        _skipHeader(stream);
        return _parseTriangleCount(stream);
    }

private:
    static void _skipHeader(StringStream &stream)
    {
        if (!stream.canExtract(80))
        {
            throw std::runtime_error("Expected 80 bytes header at the beginning of the file");
        }
        stream.extract(80);
    }

    static size_t _parseTriangleCount(StringStream &stream)
    {
        if (!stream.canExtract(4))
        {
            throw std::runtime_error("Expected 4 bytes triangle count");
        }
        return Parse::fromBytes<uint32_t>(stream, Endian::Little);
    }
};

class BinaryFacetParser
{
public:
    static std::vector<Facet> parseAll(StringStream &stream, size_t count)
    {
        std::vector<Facet> facets;
        facets.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            auto facet = parse(stream);
            facets.push_back(facet);
        }
        return facets;
    }

    static Facet parse(StringStream &stream)
    {
        Facet facet;
        Parse::fromBytes(stream, Endian::Little, facet.normal);
        Parse::fromBytes(stream, Endian::Little, facet.vertices);
        stream.extract(2);
        return facet;
    }
};

class BinarySolidParser
{
public:
    static Solid parse(std::string_view data)
    {
        auto stream = StringStream(data);
        auto count = BinaryHeaderParser::parse(stream);
        _checkDataSize(stream, count);
        return _extractSolid(stream, count);
    }

private:
    static void _checkDataSize(StringStream &stream, size_t count)
    {
        auto size = 50 * count;
        if (!stream.canExtract(size))
        {
            throw std::runtime_error("Expected " + std::to_string(size) + " bytes of facet data after header");
        }
    }

    static Solid _extractSolid(StringStream &stream, size_t count)
    {
        Solid solid;
        solid.facets = BinaryFacetParser::parseAll(stream, count);
        return solid;
    }
};

class MeshConverter
{
public:
    static TriangleMesh convert(const Solid &solid)
    {
        TriangleMesh mesh;
        _reserve(solid, mesh);
        _addVertices(solid, mesh);
        _addIndices(mesh);
        return mesh;
    }

private:
    static void _reserve(const Solid &solid, TriangleMesh &mesh)
    {
        auto size = 3 * solid.facets.size();
        mesh.vertices.reserve(size);
        mesh.normals.reserve(size);
        mesh.indices.reserve(size);
    }

    static void _addVertices(const Solid &solid, TriangleMesh &mesh)
    {
        for (const auto &facet : solid.facets)
        {
            auto &normal = facet.normal;
            for (const auto &vertex : facet.vertices)
            {
                mesh.vertices.push_back(vertex);
                mesh.normals.push_back(normal);
            }
        }
    }

    static void _addIndices(TriangleMesh &mesh)
    {
        auto size = mesh.vertices.size();
        if (size > std::numeric_limits<uint32_t>::max())
        {
            throw std::runtime_error("Too many vertices");
        }
        auto count = uint32_t(size);
        for (uint32_t i = 0; i < count; i += 3)
        {
            mesh.indices.emplace_back(i, i + 1, i + 2);
        }
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> StlMeshParser::getSupportedExtensions() const
{
    return {"stl"};
}

TriangleMesh StlMeshParser::parse(std::string_view data) const
{
    auto solid = Format::isAscii(data) ? AsciiSolidParser::parse(data) : BinarySolidParser::parse(data);
    return MeshConverter::convert(solid);
}
} // namespace brayns
