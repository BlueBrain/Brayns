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

#include "PlyMeshParser.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <optional>
#include <stdexcept>
#include <variant>

#include <brayns/common/Log.h>

#include "helpers/BinaryHelper.h"
#include "helpers/EndianHelper.h"
#include "helpers/StreamHelper.h"
#include "helpers/StringHelper.h"

namespace
{
using namespace brayns;

enum class Format
{
    Unknown,
    Ascii,
    BinaryLittleEndian,
    BinaryBigEndian
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
        if (name == "binary_little_endian")
        {
            return Format::BinaryLittleEndian;
        }
        if (name == "binary_big_endian")
        {
            return Format::BinaryBigEndian;
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
        if (property == "u" || property == "s" || property == "tx" ||
            property == "texture_u")
        {
            return Semantic::TextureX;
        }
        if (property == "v" || property == "t" || property == "ty" ||
            property == "texture_v")
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

    bool isList() const { return countType != Type::Unknown; }
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
        return format == Format::BinaryLittleEndian ||
               format == Format::BinaryBigEndian;
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

struct Context
{
    std::string_view data;
    size_t lineNumber = 0;
    std::string_view line;
    std::string_view key;
    std::string_view value;
    Header header;
    const Element *element = nullptr;
    const Property *property = nullptr;
    MeshBuffer mesh;

    Context(std::string_view source)
        : data(source)
    {
    }
};

class FormatParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "format";
    }

    static void parse(Context &context)
    {
        auto &header = context.header;
        auto value = context.value;
        _parseFormat(value, header);
        _parseVersion(value, header);
    }

private:
    static void _parseFormat(std::string_view &value, Header &header)
    {
        if (header.format != Format::Unknown)
        {
            throw std::runtime_error("Format defined twice");
        }
        auto format = StringHelper::extractToken(value);
        header.format = GetFormat::fromName(format);
    }

    static void _parseVersion(std::string_view &value, Header &header)
    {
        auto version = StringHelper::extractToken(value);
        if (version != "1.0")
        {
            throw std::runtime_error("Unsupported version");
        }
        header.version = version;
    }
};

class ElementParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "element";
    }

    static bool parse(Context &context)
    {
        if (StringHelper::countTokens(context.value) != 2)
        {
            throw std::runtime_error("Expected name and count for an element");
        }
        _addElement(context);
    }

private:
    static void _addElement(Context &context)
    {
        auto value = context.value;
        auto &header = context.header;
        auto &element = header.elements.emplace_back();
        auto name = StringHelper::extractToken(value);
        element.name = name;
        element.count = StringHelper::extract<size_t>(value);
        element.semantic = GetElementSemantic::fromName(name);
    }
};

class PropertyParser
{
public:
    static bool canParse(const Context &context)
    {
        return context.key == "property";
    }

    static void parse(Context &context)
    {
        auto count = StringHelper::countTokens(context.value);
        if (count == 2)
        {
            _parseScalar(context);
            return;
        }
        if (count == 4)
        {
            _parseList(context);
            return;
        }
        throw std::runtime_error("Invalid token count for property " +
                                 std::to_string(count));
    }

private:
    static void _parseScalar(Context &context)
    {
        auto data = context.value;
        auto &property = _createProperty(context);
        _parseProperty(data, property);
    }

    static void _parseList(Context &context)
    {
        auto data = context.value;
        auto &property = _createProperty(context);
        auto key = StringHelper::extractToken(data);
        if (key != "list")
        {
            throw std::runtime_error(
                "Expected 'list' key before property with 4 tokens");
        }
        auto countType = StringHelper::extractToken(data);
        property.countType = GetType::fromName(countType);
        _parseProperty(data, property);
    }

    static Element &_getCurrentElement(Context &context)
    {
        auto &header = context.header;
        auto &elements = header.elements;
        if (elements.empty())
        {
            throw std::runtime_error("Property without element");
        }
        return elements.back();
    }

    static Property &_createProperty(Context &context)
    {
        auto &element = _getCurrentElement(context);
        auto &properties = element.properties;
        return properties.emplace_back();
    }

    static void _parseProperty(std::string_view &data, Property &property)
    {
        auto type = StringHelper::extractToken(data);
        property.type = GetType::fromName(type);
        auto name = StringHelper::extractToken(data);
        property.name = name;
        property.semantic = GetSemantic::fromName(name);
    }
};

class HeaderValidator
{
public:
    static void validate(const Header &header)
    {
        _checkFormat(header);
        _checkDuplication(header);
    }

private:
    static void _checkFormat(const Header &header)
    {
        if (header.format == Format::Unknown)
        {
            throw std::runtime_error("Header format not specified");
        }
    }

    static void _checkDuplication(const Header &header)
    {
        std::bitset<size_t(ElementSemantic::Count)> semantics;
        for (const auto &element : header.elements)
        {
            auto semantic = element.semantic;
            if (semantic == ElementSemantic::Unknown)
            {
                Log::debug("Unknow semantic for element {}.", element.name);
                continue;
            }
            auto index = size_t(semantic);
            if (!semantics[index])
            {
                semantics[index] = true;
                continue;
            }
            std::string name = {element.name.data(), element.name.size()};
            throw std::runtime_error("Semantic duplication '" + name + "'");
            _checkDuplication(element);
        }
    }

    static void _checkDuplication(const Element &element)
    {
        std::bitset<size_t(Semantic::Count)> semantics;
        for (const auto &property : element.properties)
        {
            auto semantic = property.semantic;
            if (semantic == Semantic::Unknown)
            {
                Log::debug("Unknow semantic for property {}.", property.name);
                continue;
            }
            auto index = size_t(semantic);
            if (!semantics[index])
            {
                semantics[index] = true;
                continue;
            }
            std::string name = {property.name.data(), property.name.size()};
            throw std::runtime_error("Semantic duplication '" + name + "'");
        }
    }
};

class HeaderParser
{
public:
    static void parse(Context &context)
    {
        _parseMagicNumber(context);
        while (_extractLine(context))
        {
            _parseLine(context);
        }
        _prepareBodyExtraction(context);
    }

private:
    static void _parseMagicNumber(Context &context)
    {
        _extractLine(context);
        if (context.line != "ply")
        {
            throw std::runtime_error("Not a PLY file");
        }
    }

    static bool _extractLine(Context &context)
    {
        if (!StreamHelper::getLine(context.data, context.line))
        {
            throw std::runtime_error("Incomplete header");
        }
        ++context.lineNumber;
        std::string_view line = context.line;
        context.key = StringHelper::extractToken(line);
        context.value = line;
        return context.key != "end_header";
    }

    static void _parseLine(Context &context)
    {
        if (StringHelper::isSpace(context.line))
        {
            return;
        }
        if (!_tryParseLine(context))
        {
            Log::debug("Skip header line {}: '{}'.", context.lineNumber,
                       context.line);
        }
    }

    static bool _tryParseLine(Context &context)
    {
        return _tryParseWith<FormatParser>(context) ||
               _tryParseWith<ElementParser>(context) ||
               _tryParseWith<PropertyParser>(context);
    }

    static void _prepareBodyExtraction(Context &context)
    {
        auto &header = context.header;
        HeaderValidator::validate(header);
        context.key = {};
        context.value = {};
        if (header.isBinary())
        {
            context.line = "<binary>";
            context.value = context.data;
        }
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

class Value
{
public:
    Value() = default;

    Value(int8_t value)
        : _value(int32_t(value))
    {
    }

    Value(uint8_t value)
        : _value(uint32_t(value))
    {
    }

    Value(int16_t value)
        : _value(int32_t(value))
    {
    }

    Value(uint16_t value)
        : _value(uint32_t(value))
    {
    }

    Value(int32_t value)
        : _value(value)
    {
    }

    Value(uint32_t value)
        : _value(value)
    {
    }

    Value(float value)
        : _value(value)
    {
    }

    Value(double value)
        : _value(value)
    {
    }

    template <typename T>
    bool is() const
    {
        return std::holds_alternative<T>(_value);
    }

    template <typename T>
    T as() const
    {
        return std::get<T>(_value);
    }

private:
    std::variant<int32_t, uint32_t, float, double> _value;
};

class ConvertValue
{
public:
    static float toFloat(Value value)
    {
        if (value.is<int32_t>())
        {
            return float(value.as<int32_t>());
        }
        if (value.is<uint32_t>())
        {
            return float(value.as<uint32_t>());
        }
        if (value.is<float>())
        {
            return value.as<float>();
        }
        if (value.is<double>())
        {
            auto asDouble = value.as<double>();
            if (asDouble > std::numeric_limits<float>::max())
            {
                throw std::runtime_error("Values must fit inside a float");
            }
            return float(asDouble);
        }
        throw std::runtime_error("Internal error");
    }

    static int32_t toSignedInteger(Value value)
    {
        if (value.is<int32_t>())
        {
            return value.as<int32_t>();
        }
        if (value.is<uint32_t>())
        {
            auto asUnsigned = value.as<uint32_t>();
            if (asUnsigned > uint32_t(std::numeric_limits<int32_t>::max()))
            {
                throw std::runtime_error("Value should fit in int32");
            }
            return int32_t(asUnsigned);
        }
        throw std::runtime_error("Value cannot be float");
    }

    static uint32_t toUnsignedInteger(Value value)
    {
        if (value.is<uint32_t>())
        {
            return value.as<uint32_t>();
        }
        if (value.is<int32_t>())
        {
            auto asSigned = value.as<int32_t>();
            if (asSigned < 0)
            {
                throw std::runtime_error("Value should be positive");
            }
            return asSigned;
        }
        throw std::runtime_error("Value cannot be float");
    }
};

class ValueExtractor
{
public:
    static Value extractValue(Context &context)
    {
        auto &property = *context.property;
        return _extract(context, property.type);
    }

    static size_t extractSize(Context &context)
    {
        auto &property = *context.property;
        if (!property.isList())
        {
            throw std::runtime_error("Property is not a list");
        }
        auto value = _extract(context, property.countType);
        auto size = ConvertValue::toUnsignedInteger(value);
        return size_t(size);
    }

private:
    static Value _extract(Context &context, Type type)
    {
        switch (type)
        {
        case Type::Int8:
            return _extract<int8_t>(context);
        case Type::UInt8:
            return _extract<uint8_t>(context);
        case Type::Int16:
            return _extract<int16_t>(context);
        case Type::UInt16:
            return _extract<uint16_t>(context);
        case Type::Int32:
            return _extract<int32_t>(context);
        case Type::UInt32:
            return _extract<uint32_t>(context);
        case Type::Float32:
            return _extract<float>(context);
        case Type::Float64:
            return _extract<double>(context);
        default:
            throw std::runtime_error("Internal error");
        }
    }

    template <typename T>
    static T _extract(Context &context)
    {
        auto &header = context.header;
        auto &value = context.value;
        switch (header.format)
        {
        case Format::Ascii:
            return StringHelper::extract<T>(value);
        case Format::BinaryLittleEndian:
            return BinaryHelper::extractLittleEndian<T>(value);
        case Format::BinaryBigEndian:
            return BinaryHelper::extractBigEndian<T>(value);
        default:
            throw std::runtime_error("Invalid format");
        }
    }
};

class PropertyExtractor
{
public:
    static void extractAndDiscard(Context &context)
    {
        auto &property = *context.property;
        if (!property.isList())
        {
            ValueExtractor::extractValue(context);
            return;
        }
        auto size = ValueExtractor::extractSize(context);
        for (size_t i = 0; i < size; ++i)
        {
            ValueExtractor::extractValue(context);
        }
    }

    static float extractFloat(Context &context)
    {
        auto value = ValueExtractor::extractValue(context);
        return ConvertValue::toFloat(value);
    }

    static int32_t extractInteger(Context &context)
    {
        auto value = ValueExtractor::extractValue(context);
        return ConvertValue::toSignedInteger(value);
    }

    static uint32_t extractIndex(Context &context)
    {
        auto value = ValueExtractor::extractValue(context);
        return ConvertValue::toUnsignedInteger(value);
    }

    static Vector3ui extractFaceIndices(Context &context)
    {
        auto size = ValueExtractor::extractSize(context);
        if (size != 3)
        {
            throw std::runtime_error("Non triangular face with " +
                                     std::to_string(size) + " indices");
        }
        auto first = extractIndex(context);
        auto second = extractIndex(context);
        auto third = extractIndex(context);
        return {first, second, third};
    }

    static void extractTristrips(Context &context,
                                 std::vector<int32_t> &tristrips)
    {
        auto size = ValueExtractor::extractSize(context);
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
            auto index = extractInteger(context);
            tristrips.push_back(index);
        }
    }

    static std::array<Vector2f, 3> extractTextureCoordinates(Context &context)
    {
        std::array<Vector2f, 3> coordinates;
        auto size = ValueExtractor::extractSize(context);
        if (size != 6)
        {
            throw std::runtime_error(
                "Expected 6 coordinates for face texture, got " +
                std::to_string(size));
        }
        for (size_t i = 0; i < 3; ++i)
        {
            coordinates[i].x = extractFloat(context);
            coordinates[i].y = extractFloat(context);
        }
        return coordinates;
    }
};

class ColorExtractor
{
public:
    static float extractAndNormalize(Context &context)
    {
        auto value = PropertyExtractor::extractFloat(context);
        auto &property = *context.property;
        auto type = property.type;
        return _normalize(value, type);
    }

private:
    static float _normalize(float value, Type type)
    {
        auto baseValue = _getBaseValue(type);
        auto range = _getRange(type);
        return baseValue + value / range;
    }

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
            return float(std::numeric_limits<uint8_t>::max());
        case Type::Int16:
        case Type::UInt16:
        case Type::UInt32:
            return float(std::numeric_limits<uint16_t>::max());
        default:
            return 1.0f;
        }
    }
};

class MeshExtractor
{
public:
    static void extract(Context &context)
    {
        auto &element = *context.element;
        switch (element.semantic)
        {
        case ElementSemantic::Vertex:
            return _extractVertex(context);
        case ElementSemantic::Tristrips:
            return _extractTristrips(context);
        case ElementSemantic::Face:
            return _extractFace(context);
        default:
            PropertyExtractor::extractAndDiscard(context);
        }
    }

private:
    static void _extractVertex(Context &context)
    {
        auto &property = *context.property;
        auto &mesh = context.mesh;
        switch (property.semantic)
        {
        case Semantic::PositionX:
            return _extractFloat(context, mesh.xs);
        case Semantic::PositionY:
            return _extractFloat(context, mesh.ys);
        case Semantic::PositionZ:
            return _extractFloat(context, mesh.zs);
        case Semantic::NormalX:
            return _extractFloat(context, mesh.nxs);
        case Semantic::NormalY:
            return _extractFloat(context, mesh.nys);
        case Semantic::NormalZ:
            return _extractFloat(context, mesh.nzs);
        case Semantic::TextureX:
            return _extractFloat(context, mesh.txs);
        case Semantic::TextureY:
            return _extractFloat(context, mesh.tys);
        case Semantic::Red:
            return _extractColor(context, mesh.rs);
        case Semantic::Green:
            return _extractColor(context, mesh.gs);
        case Semantic::Blue:
            return _extractColor(context, mesh.bs);
        case Semantic::Alpha:
            return _extractColor(context, mesh.as);
        default:
            return PropertyExtractor::extractAndDiscard(context);
        }
    }

    static void _extractFace(Context &context)
    {
        auto &property = *context.property;
        switch (property.semantic)
        {
        case Semantic::VertexIndices:
            return _extractFaceIndices(context);
        case Semantic::TextureCoordinates:
            return _extractTextureCoordinates(context);
        default:
            return PropertyExtractor::extractAndDiscard(context);
        }
    }

    static void _extractTristrips(Context &context)
    {
        auto &property = *context.property;
        switch (property.semantic)
        {
        case Semantic::VertexIndices:
            return _extractTristripsIndices(context);
        default:
            return PropertyExtractor::extractAndDiscard(context);
        }
    }

    static void _extractFloat(Context &context, std::vector<float> &to)
    {
        auto value = PropertyExtractor::extractFloat(context);
        to.push_back(value);
    }

    static void _extractColor(Context &context, std::vector<float> &to)
    {
        auto value = ColorExtractor::extractAndNormalize(context);
        to.push_back(value);
    }

    static void _extractFaceIndices(Context &context)
    {
        auto &mesh = context.mesh;
        auto &faces = mesh.faces;
        auto value = PropertyExtractor::extractFaceIndices(context);
        faces.push_back(value);
    }

    static void _extractTristripsIndices(Context &context)
    {
        auto &mesh = context.mesh;
        auto &tristrips = mesh.tristrips;
        PropertyExtractor::extractTristrips(context, tristrips);
    }

    static void _extractTextureCoordinates(Context &context)
    {
        auto &mesh = context.mesh;
        auto &textures = mesh.textures;
        auto value = PropertyExtractor::extractTextureCoordinates(context);
        textures.push_back(value);
    }
};

class BodyLine
{
public:
    static void next(Context &context)
    {
        if (context.header.isBinary())
        {
            return;
        }
        _nextNonEmptyLine(context);
    }

private:
    static void _nextNonEmptyLine(Context &context)
    {
        while (true)
        {
            _nextLine(context);
            if (!StringHelper::isSpace(context.value))
            {
                return;
            }
        }
    }

    static void _nextLine(Context &context)
    {
        if (!StreamHelper::getLine(context.data, context.line))
        {
            throw std::runtime_error("Incomplete data");
        }
        context.value = context.line;
        ++context.lineNumber;
    }
};

class BodyParser
{
public:
    static void parse(Context &context)
    {
        auto &header = context.header;
        for (const auto &element : header.elements)
        {
            context.element = &element;
            for (size_t i = 0; i < element.count; ++i)
            {
                BodyLine::next(context);
                for (const auto &property : element.properties)
                {
                    context.property = &property;
                    MeshExtractor::extract(context);
                }
            }
        }
    }
};

class ErrorMessage
{
public:
    static std::string format(const Context &context,
                              const std::string &message)
    {
        std::ostringstream stream;
        stream << "Parsing error at line " << context.lineNumber;
        stream << ": '" << message << "'";
        stream << ". Line content: '" << context.line << "'";
        if (context.element)
        {
            stream << ". Element: '" << context.element->name << "'";
        }
        if (context.property)
        {
            stream << ". Property: '" << context.property->name << "'";
        }
        return stream.str();
    }
};

class PlyParser
{
public:
    static MeshBuffer parse(std::string_view data)
    {
        Context context(data);
        _tryParse(context);
        return context.mesh;
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
        HeaderParser::parse(context);
        BodyParser::parse(context);
    }
};

class VectorHelper
{
public:
    static float get(const std::vector<float> &from, size_t index,
                     float defaultValue = 0.0f)
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

class ConvertTristrips
{
public:
    static void toTriangles(const std::vector<int32_t> &tristrips,
                            std::vector<Vector3ui> &indices)
    {
        for (size_t i = 0; i < tristrips.size(); ++i)
        {
            auto size = _getStripSize(tristrips, i);
            if (size < 3)
            {
                throw std::runtime_error("Invalid triangle strip size: " +
                                         std::to_string(size));
            }
            _loadTriangles(tristrips, i, size, indices);
            i += size;
        }
    }

private:
    static size_t _getStripSize(const std::vector<int32_t> &tristrips,
                                size_t offset)
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

    static void _loadTriangles(const std::vector<int32_t> &tristrips,
                               size_t offset, size_t size,
                               std::vector<Vector3ui> &indices)
    {
        for (size_t i = 0; i < size - 2; i++)
        {
            auto &triangle = indices.emplace_back();
            triangle[0] = tristrips[offset + i];
            triangle[1] = tristrips[offset + i + 1];
            triangle[2] = tristrips[offset + i + 2];
            if (i & 1)
            {
                std::swap(triangle[0], triangle[1]);
            }
        }
    }
};

class MeshConverter
{
public:
    static TriangleMesh convert(const MeshBuffer &buffer)
    {
        TriangleMesh mesh;
        _getIndices(buffer, mesh);
        _getPositions(buffer, mesh);
        _getNormals(buffer, mesh);
        _getColors(buffer, mesh);
        _getTextures(buffer, mesh);
        return mesh;
    }

private:
    static void _getIndices(const MeshBuffer &buffer, TriangleMesh &mesh)
    {
        mesh.indices = buffer.faces;
        ConvertTristrips::toTriangles(buffer.tristrips, mesh.indices);
    }

    static void _getPositions(const MeshBuffer &buffer, TriangleMesh &mesh)
    {
        if (buffer.xs.empty() && buffer.ys.empty() && buffer.zs.empty())
        {
            return;
        }
        mesh.vertices.reserve(3 * mesh.indices.size());
        for (const auto &triangle : mesh.indices)
        {
            for (auto index : triangle)
            {
                mesh.vertices.emplace_back(VectorHelper::get(buffer.xs, index),
                                           VectorHelper::get(buffer.ys, index),
                                           VectorHelper::get(buffer.zs, index));
            }
        }
    }

    static void _getNormals(const MeshBuffer &buffer, TriangleMesh &mesh)
    {
        if (buffer.nxs.empty() && buffer.nys.empty() && buffer.nzs.empty())
        {
            return;
        }
        mesh.normals.reserve(3 * mesh.indices.size());
        for (const auto &triangle : mesh.indices)
        {
            for (auto index : triangle)
            {
                mesh.normals.emplace_back(VectorHelper::get(buffer.nxs, index),
                                          VectorHelper::get(buffer.nys, index),
                                          VectorHelper::get(buffer.nzs, index));
            }
        }
    }

    static void _getColors(const MeshBuffer &buffer, TriangleMesh &mesh)
    {
        if (buffer.rs.empty() && buffer.gs.empty() && buffer.bs.empty() &&
            buffer.as.empty())
        {
            return;
        }
        mesh.colors.reserve(3 * mesh.indices.size());
        for (const auto &triangle : mesh.indices)
        {
            for (auto index : triangle)
            {
                mesh.colors.emplace_back(VectorHelper::get(buffer.rs, index),
                                         VectorHelper::get(buffer.gs, index),
                                         VectorHelper::get(buffer.bs, index),
                                         VectorHelper::get(buffer.as, index,
                                                           1.0f));
            }
        }
    }

    static void _getTextures(const MeshBuffer &buffer, TriangleMesh &mesh)
    {
        if (buffer.txs.empty() && buffer.tys.empty())
        {
            return _getTexturesFromFaces(buffer, mesh);
        }
        if (!buffer.textures.empty())
        {
            throw std::runtime_error(
                "Texture coordinates defined in both face and vertices");
        }
        mesh.textureCoordinates.reserve(3 * mesh.indices.size());
        for (const auto &triangle : mesh.indices)
        {
            for (auto index : triangle)
            {
                mesh.textureCoordinates.emplace_back(
                    VectorHelper::get(buffer.txs, index),
                    VectorHelper::get(buffer.tys, index));
            }
        }
    }

    static void _getTexturesFromFaces(const MeshBuffer &buffer,
                                      TriangleMesh &mesh)
    {
        if (buffer.textures.empty())
        {
            return;
        }
        if (buffer.textures.size() != mesh.indices.size())
        {
            throw std::runtime_error(
                "Indices and texture coordinates mismatch");
        }
        mesh.textureCoordinates.reserve(3 * buffer.textures.size());
        for (const auto &texture : buffer.textures)
        {
            mesh.textureCoordinates.emplace_back(texture[0]);
            mesh.textureCoordinates.emplace_back(texture[1]);
            mesh.textureCoordinates.emplace_back(texture[2]);
        }
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
