/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "StbiHelper.h"

#include <array>
#include <cassert>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <brayns/core/utils/parsing/Parser.h>

#pragma GCC diagnostic pop

namespace
{
using namespace brayns;

class XmpXmlMetadataBuilder
{
public:
    static std::string build(const ImageMetadata &metadata)
    {
        auto xmlTitle = "<photoshop:Headline>" + metadata.title + "</photoshop:Headline>";
        auto xmlDescr = "<dc:description>" + metadata.description + "</dc:description>";
        auto xmlWhereUsed = "<Iptc4xmpExt:Event>" + metadata.whereUsed + "</Iptc4xmpExt:Event>";
        auto xmlKeywords = "<dc:subject>" + _serializeList(metadata.keywords) + "</dc:subject>";
        return xmlTitle + xmlDescr + xmlWhereUsed + xmlKeywords;
    }

private:
    static std::string _serializeList(const std::vector<std::string> &list)
    {
        constexpr auto bagSchemaSize = std::char_traits<char>::length("<rdf:Bag></rdf:Bag>");
        constexpr auto itemSchemaSize = std::char_traits<char>::length("<rdf:li></rdf:li>");

        std::size_t itemsSize = 0;
        for (auto &item : list)
        {
            itemsSize += item.size();
        }

        std::size_t resultSize = bagSchemaSize + itemsSize + itemSchemaSize * list.size();
        std::string result;
        result.reserve(resultSize);

        result += "<rdf:Bag>";
        for (auto &item : list)
        {
            result += "<rdf:li>" + item + "</rdf:li>";
        }
        result += "</rdf:Bag>";

        return result;
    }
};

template<typename T>
concept Number = std::is_integral_v<T> || std::is_same_v<uint8_t, T>;

template<typename T>
concept String = std::is_same_v<std::string, T> || std::is_same_v<std::string_view, T>;

class ByteSerializer
{
public:
    static void serialize(Number auto element, std::string &buffer)
    {
        // Both Jpeg and Png are stored as big endian
        auto endianCorrectElement = ByteConverter::convertFromLocalEndian(element, std::endian::big);
        auto castedElement = ByteConverter::getBytes(endianCorrectElement);
        buffer.insert(buffer.end(), castedElement, castedElement + sizeof(element));
    }

    static void serialize(const String auto &string, std::string &buffer)
    {
        buffer += string;
    }
};

class JpegXmpMarkerEncoder
{
public:
    static std::string encode(const ImageMetadata &metadata)
    {
        constexpr uint16_t maxPacketSize = 65503;

        constexpr uint16_t identifier = 0xFFE1;
        constexpr std::string_view namespaceName = "http://ns.adobe.com/xap/1.0/";
        auto packet = XmpXmlMetadataBuilder::build(metadata);

        if (packet.size() > maxPacketSize)
        {
            throw std::invalid_argument("Jpeg Xmp metadata lenght cannot exceed " + std::to_string(maxPacketSize));
        }

        uint16_t length = static_cast<uint16_t>(2 + namespaceName.size() + 1 + packet.size() + 1);

        std::string result;
        result.reserve(2 + length);

        ByteSerializer::serialize(identifier, result);
        ByteSerializer::serialize(length, result);
        ByteSerializer::serialize(namespaceName, result);
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // null terminator
        ByteSerializer::serialize(packet, result);
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // null terminator

        return result;
    }
};

class JpegXmpMarkerInserter
{
public:
    static void insertInto(const std::string &marker, std::string &image)
    {
        // The marker must be inserterd before the SOF marker
        auto insertIndex = _searchSofMarker(image);
        image.insert(insertIndex, marker);
    }

private:
    static size_t _searchSofMarker(const std::string &image)
    {
        auto bytes = reinterpret_cast<const uint8_t *>(image.data());
        size_t index = 2; // Skip SOI marker bytes

        while (index < image.size())
        {
            if (index + 4 >= image.size())
            {
                throw std::invalid_argument("Incomplete marker at end of file");
            }

            auto marker = bytes + index;

            auto idByte1 = marker[0];
            auto idByte2 = marker[1];
            auto size = Parser::parseBytes<uint16_t>({image.data() + index + 2, 2}, std::endian::big);

            if (idByte1 == 0xFF && idByte2 == 0xC0)
            {
                return index;
            }

            index += 2 + size;
        }

        throw std::invalid_argument("Could not find SOF marker");
    }
};

// https://www.w3.org/TR/PNG-Structure.html#CRC-algorithm
class CrcTableCalculator
{
public:
    static constexpr std::array<uint32_t, 256> makeCrcTable()
    {
        auto table = std::array<uint32_t, 256>();

        for (uint32_t n = 0; n < 256; ++n)
        {
            auto c = n;
            for (size_t k = 0; k < 8; ++k)
            {
                if (c & 1)
                {
                    c = 0xedb88320L ^ (c >> 1);
                    continue;
                }
                c = c >> 1;
            }

            table[n] = c;
        }

        return table;
    }
};

class PngCrcCalculator
{
public:
    static uint32_t crc(char *buf, size_t len)
    {
        return _updateCrc(0xffffffffL, buf, len) ^ 0xffffffffL;
    }

private:
    static uint32_t _updateCrc(uint32_t crc, char *buf, size_t len)
    {
        for (size_t n = 0; n < len; ++n)
        {
            crc = _crcTable[(crc ^ buf[n]) & 0xff] ^ (crc >> 8);
        }
        return crc;
    }

private:
    static constexpr std::array<uint32_t, 256> _crcTable = CrcTableCalculator::makeCrcTable();
};

class PngXmpChunkEncoder
{
public:
    static std::string encode(const ImageMetadata &metadata)
    {
        const std::string_view chunkType = "iTXt";
        constexpr std::string_view keyword = "XML:com.adobe.xmp";

        auto chunkText = XmpXmlMetadataBuilder::build(metadata);

        auto size = keyword.size() + 5 + chunkText.size();
        auto realSize = size + 4 + chunkType.size() + 4;

        std::string result;
        result.reserve(realSize);

        ByteSerializer::serialize(static_cast<uint32_t>(size), result);
        ByteSerializer::serialize(chunkType, result);
        ByteSerializer::serialize(keyword, result);
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // null separator
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // compression flag
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // compression method
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // null separator
        ByteSerializer::serialize(static_cast<uint8_t>(0), result); // null separator
        ByteSerializer::serialize(chunkText, result);

        auto crc = PngCrcCalculator::crc(result.data() + 4, result.size() - 4);
        ByteSerializer::serialize(crc, result);

        return result;
    }
};

class PngXmpChunkInserter
{
public:
    static void insertInto(const std::string &chunk, std::string &image)
    {
        // Last 12 bytes are the end of image chunk.
        image.insert(image.size() - 12, chunk);
    }
};

class StbiDecoder
{
public:
    static Image decode(const void *data, size_t size)
    {
        return decode(static_cast<const unsigned char *>(data), int(size));
    }

    static Image decode(const unsigned char *data, int size)
    {
        int width, height, channelCount;
        auto pixels = stbi_load_from_memory(data, size, &width, &height, &channelCount, 0);
        auto info = _getImageInfo(width, height, channelCount);
        auto copy = _copyPixels(pixels, info.getSize());
        stbi_image_free(pixels);
        return {info, copy};
    }

private:
    static ImageInfo _getImageInfo(int width, int height, int channelCount)
    {
        ImageInfo info;
        info.width = size_t(width);
        info.height = size_t(height);
        info.channelCount = size_t(channelCount);
        info.channelSize = 1;
        return info;
    }

    static std::string _copyPixels(unsigned char *pixels, size_t count)
    {
        auto data = reinterpret_cast<const char *>(pixels);
        return {data, count};
    }
};

class StringBuilder
{
public:
    static void append(void *context, void *data, int size)
    {
        auto &buffer = *static_cast<std::string *>(context);
        auto str = static_cast<const char *>(data);
        buffer.append(str, size_t(size));
    }
};

class StbiPngEncoder
{
public:
    static std::string encode(const Image &image, const std::optional<ImageMetadata> &metadata)
    {
        std::string context;
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        assert(image.getChannelSize() == 1);
        auto data = image.getData();
        auto success = stbi_write_png_to_func(&StringBuilder::append, &context, width, height, channelCount, data, 0);
        if (!success)
        {
            return {};
        }

        if (metadata)
        {
            _embedMetadata(*metadata, context);
        }

        return context;
    }

private:
    static void _embedMetadata(const ImageMetadata &metadata, std::string &image)
    {
        auto chunk = PngXmpChunkEncoder::encode(metadata);
        PngXmpChunkInserter::insertInto(chunk, image);
    }
};

class StbiJpegEncoder
{
public:
    static std::string encode(const Image &image, int quality, const std::optional<ImageMetadata> &metadata)
    {
        std::string context;
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        assert(image.getChannelSize() == 1);
        auto data = image.getData();
        auto success =
            stbi_write_jpg_to_func(&StringBuilder::append, &context, width, height, channelCount, data, quality);
        if (!success)
        {
            return {};
        }

        if (metadata)
        {
            _embedMetadata(*metadata, context);
        }

        return context;
    }

private:
    static void _embedMetadata(const brayns::ImageMetadata &metadata, std::string &image)
    {
        auto marker = JpegXmpMarkerEncoder::encode(metadata);
        JpegXmpMarkerInserter::insertInto(marker, image);
    }
};
} // namespace

namespace brayns
{
Image StbiHelper::decode(const void *data, size_t size)
{
    return StbiDecoder::decode(data, size);
}

std::string StbiHelper::encodePng(const Image &image, const std::optional<ImageMetadata> &metadata)
{
    return StbiPngEncoder::encode(image, metadata);
}

std::string StbiHelper::encodeJpeg(const Image &image, int quality, const std::optional<ImageMetadata> &metadata)
{
    return StbiJpegEncoder::encode(image, quality, metadata);
}
} // namespace brayns
