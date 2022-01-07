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

#include "StlMeshParser.h"

#include <array>

#include "helpers/BinaryHelper.h"
#include "helpers/StreamHelper.h"
#include "helpers/StringHelper.h"

namespace
{
using namespace brayns;

struct Facet
{
    Vector3f normal{0.0f};
    std::array<Vector3f, 3> vertices;
};

struct Solid
{
    std::string name;
    std::vector<Facet> facets;
};

struct Context
{
    std::string_view data;
    size_t lineNumber = 0;
    std::string_view line;
    std::string_view key;
    std::string_view value;
    bool facet = false;
    bool loop = false;
    bool end = false;
    size_t vertex = 0;
    Solid solid;

    Context(std::string_view source)
        : data(source)
    {
    }
};

class Format
{
public:
    static bool isAscii(std::string_view data)
    {
        std::string_view line;
        StreamHelper::getLine(data, line);
        auto solid = StringHelper::extractToken(line);
        if (solid != "solid")
        {
            return false;
        }
        while (StreamHelper::getLine(data, line))
        {
            if (StringHelper::isSpace(line))
            {
                continue;
            }
            auto facet = StringHelper::extractToken(line);
            auto normal = StringHelper::extractToken(line);
            return facet == "facet" && normal == "normal";
        }
        throw std::runtime_error("Empty file");
    }
};

class CurrentFacet
{
public:
    static Facet &get(Context &context)
    {
        auto &solid = context.solid;
        auto &facets = solid.facets;
        if (facets.empty())
        {
            throw std::runtime_error("No current facet");
        }
        return facets.back();
    }

    static Facet &next(Context &context)
    {
        auto &solid = context.solid;
        auto &facets = solid.facets;
        return facets.emplace_back();
    }
};

class BeginSolid
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "solid";
    }

    static void parse(Context &context)
    {
        if (context.facet)
        {
            throw std::runtime_error("Opening facet before solid");
        }
        if (context.loop)
        {
            throw std::runtime_error("Opening loop before solid");
        }
        context.solid.name = context.value;
    }
};

class EndSolid
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "endsolid";
    }

    static void parse(Context &context)
    {
        if (context.facet)
        {
            throw std::runtime_error("Closing solid before facet");
        }
        if (context.loop)
        {
            throw std::runtime_error("Closing loop before solid");
        }
        context.end = true;
    }
};

class BeginFacet
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "facet";
    }

    static void parse(Context &context)
    {
        if (context.facet)
        {
            throw std::runtime_error("Opening facet twice");
        }
        if (context.loop)
        {
            throw std::runtime_error("Opening facet after loop");
        }
        context.facet = true;
        _parseNormal(context);
    }

private:
    static void _parseNormal(Context &context)
    {
        auto &value = context.value;
        auto key = StringHelper::extractToken(value);
        if (key != "normal")
        {
            throw std::runtime_error("Facet without 'normal' keyword");
        }
        auto &facet = CurrentFacet::next(context);
        auto &normal = facet.normal;
        normal.x = StringHelper::extract<float>(value);
        normal.y = StringHelper::extract<float>(value);
        normal.z = StringHelper::extract<float>(value);
    }
};

class EndFacet
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "endfacet";
    }

    static void parse(Context &context)
    {
        if (!context.facet)
        {
            throw std::runtime_error("No facet to end");
        }
        if (context.loop)
        {
            throw std::runtime_error("Ending facet before loop");
        }
        context.facet = false;
    }
};

class BeginLoop
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "outer";
    }

    static void parse(Context &context)
    {
        auto &value = context.value;
        auto token = StringHelper::extractToken(value);
        if (token != "loop")
        {
            throw std::runtime_error("Expected 'loop' after 'outer'");
        }
        if (context.loop)
        {
            throw std::runtime_error("Opening loop twice");
        }
        if (!context.facet)
        {
            throw std::runtime_error("Opening loop before facet");
        }
        context.loop = true;
    }
};

class EndLoop
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "endloop";
    }

    static void parse(Context &context)
    {
        if (!context.loop)
        {
            throw std::runtime_error("No loop to end");
        }
        if (!context.facet)
        {
            throw std::runtime_error("End loop outside facet");
        }
        if (context.vertex != 3)
        {
            throw std::runtime_error("Expected 3 vertices per facet");
        }
        context.loop = false;
        context.vertex = 0;
    }
};

class Vertex
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "vertex";
    }

    static void parse(Context &context)
    {
        if (!context.facet || !context.loop)
        {
            throw std::runtime_error("Vertex definition outside facet");
        }
        if (context.vertex >= 3)
        {
            throw std::runtime_error("More that 3 vertices in facet");
        }
        _extract(context);
        ++context.vertex;
    }

private:
    static void _extract(Context &context)
    {
        auto &value = context.value;
        auto &facet = CurrentFacet::get(context);
        auto &vertex = facet.vertices[context.vertex];
        vertex.x = StringHelper::extract<float>(value);
        vertex.y = StringHelper::extract<float>(value);
        vertex.z = StringHelper::extract<float>(value);
    }
};

class AsciiLineParser
{
public:
    static void parse(Context &context)
    {
        auto line = context.line;
        if (StringHelper::isSpace(line))
        {
            return;
        }
        context.key = StringHelper::extractToken(line);
        context.value = line;
        if (!_tryParse(context))
        {
            throw std::runtime_error("Invalid line");
        }
    }

private:
    static bool _tryParse(Context &context)
    {
        return _tryParseWith<BeginSolid>(context) ||
               _tryParseWith<EndSolid>(context) ||
               _tryParseWith<BeginFacet>(context) ||
               _tryParseWith<EndFacet>(context) ||
               _tryParseWith<BeginLoop>(context) ||
               _tryParseWith<EndLoop>(context) ||
               _tryParseWith<Vertex>(context);
    }

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
    static std::string format(const Context &context, const char *message)
    {
        std::ostringstream stream;
        stream << "Parsing error at line " << context.lineNumber;
        stream << ": '" << message << "'";
        stream << ". Line content: '" << context.line << "'";
        return stream.str();
    }
};

class AsciiParser
{
public:
    static Solid parse(std::string_view data)
    {
        Context context(data);
        try
        {
            _parse(context);
            return context.solid;
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
        while (StreamHelper::getLine(context.data, context.line))
        {
            ++context.lineNumber;
            AsciiLineParser::parse(context);
            if (context.end)
            {
                return;
            }
        }
        throw std::runtime_error("Unterminated solid");
    }
};

class BinaryHeader
{
public:
    static size_t getTriangleCount(std::string_view &data)
    {
        if (data.size() < 84)
        {
            throw std::runtime_error("Expected 80 bytes header");
        }
        data = data.substr(80);
        auto size = BinaryHelper::extractLittleEndian<uint32_t>(data);
        return size_t(size);
    }
};

class BinaryParser
{
public:
    static Solid parse(std::string_view data)
    {
        auto count = BinaryHeader::getTriangleCount(data);
        _checkDataSize(data, count);
        return _extractSolid(data, count);
    }

private:
    static void _checkDataSize(std::string_view data, size_t count)
    {
        auto size = 50 * count;
        if (data.size() < size)
        {
            throw std::runtime_error("Expected " + std::to_string(size) +
                                     " bytes of data after header");
        }
    }

    static Solid _extractSolid(std::string_view &data, size_t count)
    {
        Solid solid;
        solid.facets = _extractFacets(data, count);
        return solid;
    }

    static std::vector<Facet> _extractFacets(std::string_view &data,
                                             size_t count)
    {
        std::vector<Facet> facets;
        facets.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            auto facet = _extractFacet(data);
            facets.push_back(facet);
        }
        return facets;
    }

    static Facet _extractFacet(std::string_view &data)
    {
        Facet facet;
        facet.normal.x = BinaryHelper::extractLittleEndian<float>(data);
        facet.normal.y = BinaryHelper::extractLittleEndian<float>(data);
        facet.normal.z = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[0].x = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[0].y = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[0].z = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[1].x = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[1].y = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[1].z = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[2].x = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[2].y = BinaryHelper::extractLittleEndian<float>(data);
        facet.vertices[2].z = BinaryHelper::extractLittleEndian<float>(data);
        BinaryHelper::extractLittleEndian<uint16_t>(data);
        return facet;
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
    auto solid = Format::isAscii(data) ? AsciiParser::parse(data)
                                       : BinaryParser::parse(data);
    return MeshConverter::convert(solid);
}
} // namespace brayns
