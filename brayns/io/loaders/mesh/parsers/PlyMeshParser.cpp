/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include "PlyMeshParser.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <limits>
#include <stdexcept>

#include <brayns/utils/Log.h>

#include <brayns/utils/parsing/FileStream.h>
#include <brayns/utils/parsing/Parser.h>
#include <brayns/utils/parsing/ParsingException.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringTrimmer.h>

namespace
{
using namespace brayns;

enum class Format
{
    Unknown,
    Ascii,
    BinaryBigEndian,
    BinaryLittleEndian
};

class GetFormat
{
public:
    static Format fromName(std::string_view name)
    {
        if (name == "ascii")
        {
            return Format::Ascii;
        }
        if (name == "binary_big_endian")
        {
            return Format::BinaryBigEndian;
        }
        if (name == "binary_little_endian")
        {
            return Format::BinaryLittleEndian;
        }
        throw std::runtime_error("Invalid format '" + std::string(name) + "'");
    }
};

enum class Type
{
    Unknown,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Float32,
    Float64
};

class GetType
{
public:
    static Type fromName(std::string_view name)
    {
        if (name == "int8" || name == "char")
        {
            return Type::Int8;
        }
        if (name == "uint8" || name == "uchar")
        {
            return Type::UInt8;
        }
        if (name == "int16" || name == "short")
        {
            return Type::Int16;
        }
        if (name == "uint16" || name == "ushort")
        {
            return Type::UInt16;
        }
        if (name == "int32" || name == "int")
        {
            return Type::Int32;
        }
        if (name == "uint32" || name == "uint")
        {
            return Type::UInt32;
        }
        if (name == "float32" || name == "float")
        {
            return Type::Float32;
        }
        if (name == "float64" || name == "double" || name == "double64")
        {
            return Type::Float64;
        }
        throw std::runtime_error("Invalid type '" + std::string(name) + "'");
    }
};

enum class ElementSemantic
{
    Unknown,
    Vertex,
    Face,
    Tristrips,
    Count
};

class GetElementSemantic
{
public:
    static ElementSemantic fromName(std::string_view name)
    {
        if (name == "vertex")
        {
            return ElementSemantic::Vertex;
        }
        if (name == "face")
        {
            return ElementSemantic::Face;
        }
        if (name == "tristrips")
        {
            return ElementSemantic::Tristrips;
        }
        return ElementSemantic::Unknown;
    }
};

enum class Semantic
{
    Unknown,
    PositionX,
    PositionY,
    PositionZ,
    NormalX,
    NormalY,
    NormalZ,
    Red,
    Green,
    Blue,
    Alpha,
    TextureX,
    TextureY,
    VertexIndices,
    TextureCoordinates,
    Count
};

class GetSemantic
{
public:
    static Semantic fromName(std::string_view property)
    {
        if (property == "x")
        {
            return Semantic::PositionX;
        }
        if (property == "y")
        {
            return Semantic::PositionY;
        }
        if (property == "z")
        {
            return Semantic::PositionZ;
        }
        if (property == "nx")
        {
            return Semantic::NormalX;
        }
        if (property == "ny")
        {
            return Semantic::NormalY;
        }
        if (property == "nz")
        {
            return Semantic::NormalZ;
        }
        if (property == "red" || property == "r")
        {
            return Semantic::Red;
        }
        if (property == "green" || property == "g")
        {
            return Semantic::Green;
        }
        if (property == "blue" || property == "b")
        {
            return Semantic::Blue;
        }
        if (property == "alpha" || property == "a")
        {
            return Semantic::Alpha;
        }
        if (property == "u" || property == "s" || property == "tx" || property == "texture_u")
        {
            return Semantic::TextureX;
        }
        if (property == "v" || property == "t" || property == "ty" || property == "texture_v")
        {
            return Semantic::TextureY;
        }
        if (property == "vertex_index" || property == "vertex_indices")
        {
            return Semantic::VertexIndices;
        }
        if (property == "texcoord")
        {
            return Semantic::TextureCoordinates;
        }
        return Semantic::Unknown;
    }
};

struct Property
{
    std::string name;
    Type type = Type::Unknown;
    Type countType = Type::Unknown;
    Semantic semantic = Semantic::Unknown;

    bool isList() const
    {
        return countType != Type::Unknown;
    }
};

struct Element
{
    std::string name;
    size_t count = 0;
    ElementSemantic semantic = ElementSemantic::Unknown;
    std::vector<Property> properties;
};

struct Header
{
    Format format = Format::Unknown;
    std::string version;
    std::vector<Element> elements;

    bool isBinary() const
    {
        return format == Format::BinaryLittleEndian || format == Format::BinaryBigEndian;
    }
};

struct MeshBuffer
{
    std::vector<float> xs;
    std::vector<float> ys;
    std::vector<float> zs;
    std::vector<float> nxs;
    std::vector<float> nys;
    std::vector<float> nzs;
    std::vector<float> rs;
    std::vector<float> gs;
    std::vector<float> bs;
    std::vector<float> as;
    std::vector<float> txs;
    std::vector<float> tys;
    std::vector<Vector3ui> faces;
    std::vector<int32_t> tristrips;
    std::vector<std::array<Vector2f, 3>> textures;
};

struct HeaderLine
{
    std::string_view key;
    std::string_view value;

    bool isComment() const
    {
        return key == "comment";
    }

    bool isEmpty() const
    {
        return key.empty();
    }

    bool isEmptyOrComment() const
    {
        return isEmpty() || isComment();
    }
};

class HeaderLineParser
{
public:
    static HeaderLine parse(std::string_view data)
    {
        HeaderLine line;
        line.key = StringExtractor::extractToken(data);
        StringExtractor::extractSpaces(data);
        line.value = data;
        return line;
    }
};

class HeaderLineExtractor
{
public:
    static HeaderLine nextLine(FileStream &stream)
    {
        while (true)
        {
            if (!stream.nextLine())
            {
                throw std::runtime_error("Unterminated header");
            }
            auto data = stream.getLine();
            auto line = HeaderLineParser::parse(data);
            if (!line.isEmptyOrComment())
            {
                return line;
            }
        }
    }
};

class MagicNumberParser
{
public:
    static void skip(FileStream &stream)
    {
        auto line = HeaderLineExtractor::nextLine(stream);
        if (line.key != "ply")
        {
            throw std::runtime_error("Expected 'ply' magic number");
        }
    }
};

class FormatParser
{
public:
    static void parse(FileStream &stream, Header &header)
    {
        auto line = HeaderLineExtractor::nextLine(stream);
        if (line.key != "format")
        {
            throw std::runtime_error("Expected format definition after magic number");
        }
        auto format = line.value;
        _parseFormat(format, header);
        _parseVersion(format, header);
    }

private:
    static void _parseFormat(std::string_view &data, Header &header)
    {
        auto token = StringExtractor::extractToken(data);
        header.format = GetFormat::fromName(token);
    }

    static void _parseVersion(std::string_view &data, Header &header)
    {
        auto token = StringExtractor::extractToken(data);
        if (token != "1.0")
        {
            throw std::runtime_error("Unsupported version '" + std::string(token) + "'");
        }
        header.version = token;
    }
};

class ElementParser
{
public:
    static Element parse(std::string_view value)
    {
        auto count = StringCounter::countTokens(value);
        if (count != 2)
        {
            throw std::runtime_error("Expected name and count for an element");
        }
        Element element;
        auto name = StringExtractor::extractToken(value);
        element.name = name;
        element.semantic = GetElementSemantic::fromName(name);
        element.count = Parser::extractToken<size_t>(value);
        return element;
    }
};

class PropertyParser
{
public:
    static Property parse(std::string_view value)
    {
        auto count = StringCounter::countTokens(value);
        if (count == 2)
        {
            return _parseScalar(value);
        }
        if (count == 4)
        {
            return _parseList(value);
        }
        throw std::runtime_error("Expected 2 or 4 tokens for a property");
    }

private:
    static Property _parseScalar(std::string_view value)
    {
        Property property;
        _parse(value, property);
        return property;
    }

    static Property _parseList(std::string_view value)
    {
        Property property;
        auto list = StringExtractor::extractToken(value);
        if (list != "list")
        {
            throw std::runtime_error("Expected 'list' key before property with 4 tokens");
        }
        auto countType = StringExtractor::extractToken(value);
        property.countType = GetType::fromName(countType);
        _parse(value, property);
        return property;
    }

    static void _parse(std::string_view value, Property &property)
    {
        auto type = StringExtractor::extractToken(value);
        property.type = GetType::fromName(type);
        auto name = StringExtractor::extractToken(value);
        property.name = name;
        property.semantic = GetSemantic::fromName(name);
    }
};

class HeaderParser
{
public:
    static Header parse(FileStream &stream)
    {
        try
        {
            return _parse(stream);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(std::string("Failed to parse header: ") + e.what());
        }
    }

private:
    static Header _parse(FileStream &stream)
    {
        MagicNumberParser::skip(stream);
        Header header;
        FormatParser::parse(stream, header);
        while (true)
        {
            auto line = HeaderLineExtractor::nextLine(stream);
            if (!_parseLine(line, header))
            {
                return header;
            }
        }
    }

    static bool _parseLine(const HeaderLine &line, Header &header)
    {
        auto &[key, value] = line;
        if (key == "end_header")
        {
            return false;
        }
        if (key == "element")
        {
            auto element = ElementParser::parse(value);
            header.elements.push_back(std::move(element));
            return true;
        }
        if (key == "property")
        {
            auto &element = _getCurrentElement(header);
            auto property = PropertyParser::parse(value);
            element.properties.push_back(std::move(property));
            return true;
        }
        throw std::runtime_error("Unknown key: '" + std::string(key) + "'");
    }

    static Element &_getCurrentElement(Header &header)
    {
        auto &elements = header.elements;
        if (elements.empty())
        {
            throw std::runtime_error("Property without element");
        }
        return elements.back();
    }
};

class HeaderValidator
{
public:
    static void validate(const Header &header)
    {
        _checkDuplication(header);
    }

private:
    static void _checkDuplication(const Header &header)
    {
        std::bitset<static_cast<size_t>(ElementSemantic::Count)> semantics;
        for (const auto &element : header.elements)
        {
            auto semantic = element.semantic;
            if (semantic == ElementSemantic::Unknown)
            {
                Log::debug("Unknown semantic for element {}.", element.name);
                continue;
            }
            auto index = static_cast<size_t>(semantic);
            if (!semantics[index])
            {
                semantics[index] = true;
                continue;
            }
            throw std::runtime_error("Element semantic duplication '" + std::string(element.name) + "'");
            _checkDuplication(element);
        }
    }

    static void _checkDuplication(const Element &element)
    {
        std::bitset<static_cast<size_t>(Semantic::Count)> semantics;
        for (const auto &property : element.properties)
        {
            auto semantic = property.semantic;
            if (semantic == Semantic::Unknown)
            {
                Log::debug("Unknown semantic for property {}.", property.name);
                continue;
            }
            auto index = static_cast<size_t>(semantic);
            if (!semantics[index])
            {
                semantics[index] = true;
                continue;
            }
            throw std::runtime_error("Property semantic duplication '" + std::string(property.name) + "'");
        }
    }
};

class ConvertValue
{
public:
    static float toFloat(double value)
    {
        if (value < std::numeric_limits<float>::lowest())
        {
            throw std::runtime_error("Values must fit inside a float");
        }
        if (value > std::numeric_limits<float>::max())
        {
            throw std::runtime_error("Values must fit inside a float");
        }
        return static_cast<float>(value);
    }

    static int32_t toInteger(double value)
    {
        if (value != std::floor(value))
        {
            throw std::runtime_error("Value must be integer");
        }
        if (value > static_cast<double>(std::numeric_limits<int32_t>::max()))
        {
            throw std::runtime_error("Value should fit in int32");
        }
        return static_cast<int32_t>(value);
    }

    static uint32_t toUnsigned(double value)
    {
        if (value < 0.0)
        {
            throw std::runtime_error("Value must be positive");
        }
        if (value != std::floor(value))
        {
            throw std::runtime_error("Value must be integer");
        }
        if (value > static_cast<double>(std::numeric_limits<uint32_t>::max()))
        {
            throw std::runtime_error("Value should fit in uint32");
        }
        return static_cast<int32_t>(value);
    }
};

class ValueParser
{
public:
    static double parse(std::string_view &data, Format format, Type type)
    {
        switch (type)
        {
        case Type::Int8:
            return parse<int8_t>(data, format);
        case Type::UInt8:
            return parse<uint8_t>(data, format);
        case Type::Int16:
            return parse<int16_t>(data, format);
        case Type::UInt16:
            return parse<uint16_t>(data, format);
        case Type::Int32:
            return parse<int32_t>(data, format);
        case Type::UInt32:
            return parse<uint32_t>(data, format);
        case Type::Float32:
            return parse<float>(data, format);
        case Type::Float64:
            return parse<double>(data, format);
        default:
            throw std::runtime_error("Internal error");
        }
    }

    template<typename T>
    static T parse(std::string_view &data, Format format)
    {
        switch (format)
        {
        case Format::Ascii:
            return Parser::extractToken<T>(data);
        case Format::BinaryBigEndian:
            return Parser::extractChunk<T>(data, std::endian::big);
        case Format::BinaryLittleEndian:
            return Parser::extractChunk<T>(data, std::endian::little);
        default:
            throw std::runtime_error("Invalid format");
        }
    }
};

class ValueExtractor
{
public:
    static float extractFloat(std::string_view &data, Format format, Type type)
    {
        auto value = ValueParser::parse(data, format, type);
        return ConvertValue::toFloat(value);
    }

    static int32_t extractInteger(std::string_view &data, Format format, Type type)
    {
        auto value = ValueParser::parse(data, format, type);
        return ConvertValue::toInteger(value);
    }

    static uint32_t extractUnsigned(std::string_view &data, Format format, Type type)
    {
        auto value = ValueParser::parse(data, format, type);
        return ConvertValue::toUnsigned(value);
    }
};

class PropertyExtractor
{
public:
    static void extractAndDiscard(std::string_view &data, Format format, const Property &property)
    {
        auto type = property.type;
        if (!property.isList())
        {
            ValueParser::parse(data, format, type);
            return;
        }
        auto size = extractSize(data, format, property);
        for (size_t i = 0; i < size; ++i)
        {
            ValueParser::parse(data, format, type);
        }
    }

    static size_t extractSize(std::string_view &data, Format format, const Property &property)
    {
        if (!property.isList())
        {
            throw std::runtime_error("Expected a list property");
        }
        auto type = property.countType;
        auto size = ValueExtractor::extractUnsigned(data, format, type);
        return static_cast<size_t>(size);
    }
};

class ColorNormalizer
{
public:
    static float normalize(float value, Type type)
    {
        auto baseValue = _getBaseValue(type);
        auto range = _getRange(type);
        return baseValue + value / range;
    }

private:
    static float _getBaseValue(Type type)
    {
        switch (type)
        {
        case Type::Int8:
        case Type::Int16:
        case Type::Int32:
            return 0.5f;
        default:
            return 0.0f;
        }
    }

    static float _getRange(Type type)
    {
        switch (type)
        {
        case Type::Int8:
        case Type::UInt8:
        case Type::Int32:
            return static_cast<float>(std::numeric_limits<uint8_t>::max());
        case Type::Int16:
        case Type::UInt16:
        case Type::UInt32:
            return static_cast<float>(std::numeric_limits<uint16_t>::max());
        default:
            return 1.0f;
        }
    }
};

class VertexExtractor
{
public:
    static void extract(std::string_view &data, Format format, const Property &property, MeshBuffer &mesh)
    {
        auto type = property.type;
        switch (property.semantic)
        {
        case Semantic::PositionX:
            return _extractFloat(data, format, type, mesh.xs);
        case Semantic::PositionY:
            return _extractFloat(data, format, type, mesh.ys);
        case Semantic::PositionZ:
            return _extractFloat(data, format, type, mesh.zs);
        case Semantic::NormalX:
            return _extractFloat(data, format, type, mesh.nxs);
        case Semantic::NormalY:
            return _extractFloat(data, format, type, mesh.nys);
        case Semantic::NormalZ:
            return _extractFloat(data, format, type, mesh.nzs);
        case Semantic::TextureX:
            return _extractFloat(data, format, type, mesh.txs);
        case Semantic::TextureY:
            return _extractFloat(data, format, type, mesh.tys);
        case Semantic::Red:
            return _extractColor(data, format, type, mesh.rs);
        case Semantic::Green:
            return _extractColor(data, format, type, mesh.gs);
        case Semantic::Blue:
            return _extractColor(data, format, type, mesh.bs);
        case Semantic::Alpha:
            return _extractColor(data, format, type, mesh.as);
        default:
            return PropertyExtractor::extractAndDiscard(data, format, property);
        }
    }

private:
    static void _extractFloat(std::string_view &data, Format format, Type type, std::vector<float> &values)
    {
        auto value = ValueExtractor::extractFloat(data, format, type);
        values.push_back(value);
    }

    static void _extractColor(std::string_view &data, Format format, Type type, std::vector<float> &colors)
    {
        auto value = ValueExtractor::extractFloat(data, format, type);
        auto color = ColorNormalizer::normalize(value, type);
        colors.push_back(color);
    }
};

class FaceExtractor
{
public:
    static void extract(std::string_view &data, Format format, const Property &property, MeshBuffer &mesh)
    {
        switch (property.semantic)
        {
        case Semantic::VertexIndices:
            return _extractIndices(data, format, property, mesh);
        case Semantic::TextureCoordinates:
            return _extractTexture(data, format, property, mesh);
        default:
            return PropertyExtractor::extractAndDiscard(data, format, property);
        }
    }

private:
    static void _extractIndices(std::string_view &data, Format format, const Property &property, MeshBuffer &mesh)
    {
        auto size = PropertyExtractor::extractSize(data, format, property);
        if (size != 3)
        {
            throw std::runtime_error("Non triangular face with " + std::to_string(size) + " indices");
        }
        auto type = property.type;
        auto &indices = mesh.faces.emplace_back();
        indices.x = ValueExtractor::extractUnsigned(data, format, type);
        indices.y = ValueExtractor::extractUnsigned(data, format, type);
        indices.z = ValueExtractor::extractUnsigned(data, format, type);
    }

    static void _extractTexture(std::string_view &data, Format format, const Property &property, MeshBuffer &mesh)
    {
        auto size = PropertyExtractor::extractSize(data, format, property);
        if (size != 6)
        {
            throw std::runtime_error("Expected 6 coordinates for face texture, got " + std::to_string(size));
        }
        auto type = property.type;
        auto &texture = mesh.textures.emplace_back();
        for (size_t i = 0; i < 3; ++i)
        {
            texture[i].x = ValueExtractor::extractFloat(data, format, type);
            texture[i].y = ValueExtractor::extractFloat(data, format, type);
        }
    }
};

class TristripExtractor
{
public:
    static void extract(std::string_view &data, Format format, const Property &property, std::vector<int32_t> &tristrips)
    {
        switch (property.semantic)
        {
        case Semantic::VertexIndices:
            return _extract(data, format, property, tristrips);
        default:
            return PropertyExtractor::extractAndDiscard(data, format, property);
        }
    }

private:
    static void _extract(
        std::string_view &data,
        Format format,
        const Property &property,
        std::vector<int32_t> &tristrips)
    {
        auto size = PropertyExtractor::extractSize(data, format, property);
        if (size == 0)
        {
            return;
        }
        if (!tristrips.empty() && tristrips.back() != -1)
        {
            tristrips.push_back(-1);
        }
        tristrips.reserve(tristrips.size() + size);
        for (size_t i = 0; i < size; ++i)
        {
            auto index = ValueExtractor::extractInteger(data, format, property.type);
            tristrips.push_back(index);
        }
    }
};

class ElementExtractor
{
public:
    static void extract(std::string_view &data, Format format, const Element &element, size_t index, MeshBuffer &mesh)
    {
        for (const auto &property : element.properties)
        {
            _tryExtract(data, format, element, index, property, mesh);
        }
    }

private:
    static void _tryExtract(
        std::string_view &data,
        Format format,
        const Element &element,
        size_t index,
        const Property &property,
        MeshBuffer &mesh)
    {
        try
        {
            auto semantic = element.semantic;
            _extract(data, format, semantic, property, mesh);
        }
        catch (const std::exception &e)
        {
            std::ostringstream stream;
            stream << "Failed to extract data for element '" << element.name;
            stream << "' (index = " << index;
            stream << ") and property '" << property.name;
            stream << "': '" << e.what() << "'";
            throw std::runtime_error(stream.str());
        }
    }

    static void _extract(
        std::string_view &data,
        Format format,
        ElementSemantic semantic,
        const Property &property,
        MeshBuffer &mesh)
    {
        switch (semantic)
        {
        case ElementSemantic::Vertex:
            return VertexExtractor::extract(data, format, property, mesh);
        case ElementSemantic::Face:
            return FaceExtractor::extract(data, format, property, mesh);
        case ElementSemantic::Tristrips:
            return TristripExtractor::extract(data, format, property, mesh.tristrips);
        default:
            PropertyExtractor::extractAndDiscard(data, format, property);
        }
    }
};

class AsciiMeshExtractor
{
public:
    static MeshBuffer extract(FileStream &stream, const std::vector<Element> &elements)
    {
        MeshBuffer mesh;
        for (const auto &element : elements)
        {
            for (size_t i = 0; i < element.count; ++i)
            {
                auto line = _nextLine(stream);
                auto format = Format::Ascii;
                ElementExtractor::extract(line, format, element, i, mesh);
            }
        }
        return mesh;
    }

private:
    static std::string_view _nextLine(FileStream &stream)
    {
        if (!stream.nextLine())
        {
            throw std::runtime_error("Incomplete ASCII data");
        }
        return stream.getLine();
    }
};

class BinaryMeshExtractor
{
public:
    static MeshBuffer extract(FileStream &file, Format format, const std::vector<Element> &elements)
    {
        MeshBuffer mesh;
        auto data = file.getData();
        for (const auto &element : elements)
        {
            for (size_t i = 0; i < element.count; ++i)
            {
                ElementExtractor::extract(data, format, element, i, mesh);
            }
        }
        return mesh;
    }
};

class PlyParser
{
public:
    static MeshBuffer parse(std::string_view data)
    {
        auto stream = FileStream(data);
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
        auto header = HeaderParser::parse(stream);
        HeaderValidator::validate(header);
        auto format = header.format;
        auto &elements = header.elements;
        switch (format)
        {
        case Format::Ascii:
            return AsciiMeshExtractor::extract(stream, elements);
        case Format::BinaryLittleEndian:
        case Format::BinaryBigEndian:
            return BinaryMeshExtractor::extract(stream, format, elements);
        default:
            throw std::runtime_error("Invalid format");
        }
    }
};

class VectorHelper
{
public:
    static float get(const std::vector<float> &from, size_t index, float defaultValue = 0.0f)
    {
        if (from.empty())
        {
            return defaultValue;
        }
        if (index > from.size())
        {
            throw std::runtime_error("Invalid index " + std::to_string(index));
        }
        return from[index];
    }
};

class TristripConverter
{
public:
    static void convert(const std::vector<int32_t> &tristrips, std::vector<Vector3ui> &indices)
    {
        for (size_t i = 0; i < tristrips.size(); ++i)
        {
            auto size = _getStripSize(tristrips, i);
            if (size < 3)
            {
                throw std::runtime_error("Invalid triangle strip size: " + std::to_string(size));
            }
            _loadTriangles(tristrips, i, size, indices);
            i += size;
        }
    }

private:
    static size_t _getStripSize(const std::vector<int32_t> &tristrips, size_t offset)
    {
        for (size_t i = offset; i < tristrips.size(); ++i)
        {
            auto index = tristrips[i];
            if (index < 0)
            {
                return i - offset;
            }
        }
        return tristrips.size() - offset;
    }

    static void _loadTriangles(
        const std::vector<int32_t> &tristrips,
        size_t offset,
        size_t size,
        std::vector<Vector3ui> &indices)
    {
        for (size_t i = 0; i < size - 2; i++)
        {
            auto &triangle = indices.emplace_back();
            auto index = offset + i;
            triangle[0] = tristrips[index];
            triangle[1] = tristrips[index + 1];
            triangle[2] = tristrips[index + 2];
            if (i & 1)
            {
                std::swap(triangle[0], triangle[1]);
            }
        }
    }
};

class IndexConverter
{
public:
    static std::vector<Vector3ui> convert(const MeshBuffer &mesh)
    {
        auto indices = mesh.faces;
        TristripConverter::convert(mesh.tristrips, indices);
        return indices;
    }

    static std::vector<Vector3ui> recompute(const std::vector<Vector3f> &vertices)
    {
        std::vector<Vector3ui> indices;
        auto size = vertices.size();
        indices.reserve(size / 3);
        for (size_t i = 0; i < size; i += 3)
        {
            indices.emplace_back(i, i + 1, i + 2);
        }
        return indices;
    }
};

class VertexConverter
{
public:
    static std::vector<Vector3f> convert(const MeshBuffer &mesh, const std::vector<Vector3ui> &indices)
    {
        if (mesh.xs.empty() && mesh.ys.empty() && mesh.zs.empty())
        {
            throw std::runtime_error("No positions in vertices");
        }
        std::vector<Vector3f> vertices;
        vertices.reserve(3 * indices.size());
        for (const auto &triangle : indices)
        {
            for (std::size_t i = 0; i < 3; ++i)
            {
                auto index = triangle[i];
                auto x = VectorHelper::get(mesh.xs, index);
                auto y = VectorHelper::get(mesh.ys, index);
                auto z = VectorHelper::get(mesh.zs, index);
                vertices.emplace_back(x, y, z);
            }
        }
        return vertices;
    }
};

class NormalConverter
{
public:
    static std::vector<Vector3f> convert(const MeshBuffer &mesh, const std::vector<Vector3ui> &indices)
    {
        if (mesh.nxs.empty() && mesh.nys.empty() && mesh.nzs.empty())
        {
            return {};
        }
        std::vector<Vector3f> normals;
        normals.reserve(3 * indices.size());
        for (const auto &triangle : indices)
        {
            for (std::size_t i = 0; i < 3; ++i)
            {
                auto index = triangle[i];
                auto x = VectorHelper::get(mesh.nxs, index);
                auto y = VectorHelper::get(mesh.nys, index);
                auto z = VectorHelper::get(mesh.nzs, index);
                normals.emplace_back(x, y, z);
            }
        }
        return normals;
    }
};

class ColorConverter
{
public:
    static std::vector<Vector4f> convert(const MeshBuffer &mesh, const std::vector<Vector3ui> &indices)
    {
        if (mesh.rs.empty() && mesh.gs.empty() && mesh.bs.empty() && mesh.as.empty())
        {
            return {};
        }
        std::vector<Vector4f> colors;
        colors.reserve(3 * indices.size());
        for (const auto &triangle : indices)
        {
            for (std::size_t i = 0; i < 3; ++i)
            {
                auto index = triangle[i];
                auto r = VectorHelper::get(mesh.rs, index);
                auto g = VectorHelper::get(mesh.gs, index);
                auto b = VectorHelper::get(mesh.bs, index);
                auto a = VectorHelper::get(mesh.as, index, 1.0f);
                colors.emplace_back(r, g, b, a);
            }
        }
        return colors;
    }
};

class TextureConverter
{
public:
    static std::vector<Vector2f> convert(const MeshBuffer &mesh, const std::vector<Vector3ui> &indices)
    {
        if (mesh.txs.empty() && mesh.tys.empty())
        {
            return _fromFaces(mesh, indices);
        }
        return _fromVertices(mesh, indices);
    }

private:
    static std::vector<Vector2f> _fromFaces(const MeshBuffer &mesh, const std::vector<Vector3ui> &indices)
    {
        if (mesh.textures.empty())
        {
            return {};
        }
        if (mesh.textures.size() != indices.size())
        {
            throw std::runtime_error("Indices and texture coordinates mismatch");
        }
        std::vector<Vector2f> textures;
        textures.reserve(3 * mesh.textures.size());
        for (const auto &texture : mesh.textures)
        {
            textures.emplace_back(texture[0]);
            textures.emplace_back(texture[1]);
            textures.emplace_back(texture[2]);
        }
        return textures;
    }

    static std::vector<Vector2f> _fromVertices(const MeshBuffer &mesh, const std::vector<Vector3ui> &indices)
    {
        if (!mesh.textures.empty())
        {
            throw std::runtime_error("Texture coordinates defined in both face and vertices");
        }
        std::vector<Vector2f> textures;
        textures.reserve(3 * indices.size());
        for (const auto &triangle : indices)
        {
            for (std::size_t i = 0; i < 3; ++i)
            {
                auto index = triangle[i];
                auto x = VectorHelper::get(mesh.txs, index);
                auto y = VectorHelper::get(mesh.tys, index);
                textures.emplace_back(x, y);
            }
        }
        return textures;
    }
};

class MeshConverter
{
public:
    static TriangleMesh convert(const MeshBuffer &buffer)
    {
        TriangleMesh mesh;
        auto indices = IndexConverter::convert(buffer);
        mesh.vertices = VertexConverter::convert(buffer, indices);
        mesh.normals = NormalConverter::convert(buffer, indices);
        mesh.colors = ColorConverter::convert(buffer, indices);
        mesh.uvs = TextureConverter::convert(buffer, indices);
        mesh.indices = IndexConverter::recompute(mesh.vertices);
        return mesh;
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> PlyMeshParser::getSupportedExtensions() const
{
    return {"ply"};
}

TriangleMesh PlyMeshParser::parse(std::string_view data) const
{
    auto buffer = PlyParser::parse(data);
    return MeshConverter::convert(buffer);
}
} // namespace brayns
