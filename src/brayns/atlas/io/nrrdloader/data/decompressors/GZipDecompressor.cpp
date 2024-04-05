/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "GZipDecompressor.h"

#include <limits>
#include <stdexcept>

#include <zlib.h>

// From
//  - https://github.com/mapbox/gzip-hpp/blob/master/include/gzip/decompress.hpp
//  - https://www.lemoda.net/c/zlib-open-read/

std::string GZipDecompressor::decompress(std::string input) const
{
    if (input.empty())
    {
        throw std::invalid_argument("Empty input not allowed");
    }

    z_stream decompressInfo{};
    decompressInfo.zalloc = Z_NULL;
    decompressInfo.zfree = Z_NULL;
    decompressInfo.opaque = Z_NULL;
    decompressInfo.avail_in = 0;
    decompressInfo.next_in = Z_NULL;

    constexpr int windowBits = 15;
    constexpr int enableGZIP = 32;
    constexpr int mask = windowBits | enableGZIP;
    if (auto errorCode = inflateInit2(&decompressInfo, mask) != Z_OK)
    {
        const auto errorMessage = std::string(zError(errorCode));
        throw std::runtime_error("Call to inflateInit2() failed: " + errorMessage);
    }

    const auto inputSize = input.length();
    if (inputSize * 2 > std::numeric_limits<unsigned int>::max())
    {
        throw std::runtime_error("Cannot decompress volume data. Data size limit exceeded");
    }

    decompressInfo.next_in = reinterpret_cast<z_const Bytef *>(input.data());
    decompressInfo.avail_in = static_cast<unsigned int>(inputSize);

    std::string result;
    size_t uncompressedSize = 0;

    do
    {
        const auto nextDecompressionSize = 2 * inputSize;

        const auto maxDecompressionOutput = uncompressedSize + nextDecompressionSize;
        result.resize(maxDecompressionOutput);

        decompressInfo.avail_out = static_cast<unsigned int>(nextDecompressionSize);
        decompressInfo.next_out = reinterpret_cast<Bytef *>(&result[uncompressedSize]);

        const auto decompressResult = inflate(&decompressInfo, Z_FINISH);
        // Z_STREAM_END - input is exhausted
        // Z_BUF_ERROR - output is exhausted
        // Z_OK - well
        if (decompressResult != Z_STREAM_END && decompressResult != Z_OK && decompressResult != Z_BUF_ERROR)
        {
            std::string msg;
            if (decompressInfo.msg)
            {
                msg = std::string(decompressInfo.msg);
            }
            inflateEnd(&decompressInfo);
            throw std::runtime_error("Call to inflate() failed: " + msg);
        }

        uncompressedSize += (nextDecompressionSize - decompressInfo.avail_out);
    } while (decompressInfo.avail_out == 0);

    inflateEnd(&decompressInfo);
    result.resize(uncompressedSize);

    return result;
}
