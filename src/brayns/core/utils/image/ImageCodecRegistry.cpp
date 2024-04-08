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

#include "ImageCodecRegistry.h"

#include <brayns/core/utils/string/StringCase.h>

#include <cassert>
#include <memory>
#include <unordered_map>

#include "codecs/ExrCodec.h"
#include "codecs/JpegCodec.h"
#include "codecs/PngCodec.h"

namespace
{
using namespace brayns;

class ImageCodecMap
{
public:
    ImageCodec *find(const std::string &format) const
    {
        auto i = _codecs.find(format);
        return i == _codecs.end() ? nullptr : i->second.get();
    }

    void add(std::unique_ptr<ImageCodec> codec)
    {
        assert(codec);
        auto format = codec->getFormat();
        auto old = find(format);
        if (old)
        {
            throw std::runtime_error("Too many codecs for '" + format + "'");
        }
        _codecs.emplace(std::move(format), std::move(codec));
    }

    template<typename T, typename... Args>
    void add(Args &&...args)
    {
        add(std::make_unique<T>(std::forward<Args>(args)...));
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ImageCodec>> _codecs;
};

class ImageCodecStorage
{
public:
    static const ImageCodecMap &getCodecs()
    {
        static const auto codecs = _createCodecs();
        return codecs;
    }

private:
    static ImageCodecMap _createCodecs()
    {
        ImageCodecMap codecs;
        codecs.add<PngCodec>();
        codecs.add<JpegCodec>();
        codecs.add<ExrCodec>();
        return codecs;
    }
};
} // namespace

namespace brayns
{
const ImageCodec &ImageCodecRegistry::getCodec(const std::string &format)
{
    auto lowerFormat = StringCase::toLower(format);

    auto &codecs = ImageCodecStorage::getCodecs();
    auto codec = codecs.find(lowerFormat);
    if (!codec)
    {
        throw std::runtime_error("Format not supported: '" + format + "'");
    }
    return *codec;
}
} // namespace brayns
