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

#include "JpegCodec.h"

#include "StbiHelper.h"

namespace brayns
{
std::string JpegCodec::getFormat() const
{
    return "jpg";
}

std::string JpegCodec::encode(const Image &image, int quality) const
{
    return StbiHelper::encodeJpeg(image, quality);
}

Image JpegCodec::decode(const void *data, size_t size) const
{
    return StbiHelper::decode(data, size);
}
} // namespace brayns
