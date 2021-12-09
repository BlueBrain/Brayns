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

#include "ImageCodecRegistry.h"

#include "codecs/JpegCodec.h"
#include "codecs/PngCodec.h"

#include "ImageCodecMap.h"

namespace
{
using namespace brayns;

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
        return codecs;
    }
};
} // namespace

namespace brayns
{
bool ImageCodecRegistry::isSupported(const std::string &format)
{
    auto &codecs = ImageCodecStorage::getCodecs();
    return codecs.find(format);
}

const ImageCodec &ImageCodecRegistry::getCodec(const std::string &format)
{
    auto &codecs = ImageCodecStorage::getCodecs();
    auto codec = codecs.find(format);
    if (!codec)
    {
        throw std::runtime_error("Format not supported: '" + format + "'");
    }
    return *codec;
}
} // namespace brayns
