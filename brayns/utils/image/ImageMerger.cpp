/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "ImageMerger.h"

#include <cassert>
#include <cstring>

namespace
{
using namespace brayns;

class ImageMergeInfo
{
public:
    static ImageInfo combine(const std::vector<Image> &images)
    {
        if (images.empty())
        {
            return {};
        }
        auto info = images[0].getInfo();
        for (size_t i = 1; i < images.size(); ++i)
        {
            _add(images[i], info);
        }
        return info;
    }

private:
    static void _add(const Image &image, ImageInfo &info)
    {
        info.width += image.getWidth();
        if (image.getHeight() != info.height)
        {
            throw std::runtime_error("All images must have the same height");
        }
        if (image.getChannelCount() != info.channelCount)
        {
            throw std::runtime_error(
                "All images must have the same channel count");
        }
        if (image.getChannelSize() != info.channelSize)
        {
            throw std::runtime_error(
                "All images must have the same channel size");
        }
    }
};
} // namespace

namespace brayns
{
Image ImageMerger::merge(const std::vector<Image> &images)
{
    auto info = ImageMergeInfo::combine(images);
    Image result(info);
    size_t x = 0;
    for (const auto &image : images)
    {
        result.write(image, x);
        x += image.getWidth();
    }
    return result;
}
} // namespace brayns
