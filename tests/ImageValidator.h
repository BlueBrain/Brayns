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

#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

#include <brayns/engine/Engine.h>
#include <brayns/utils/image/Image.h>
#include <brayns/utils/image/ImageDecoder.h>
#include <brayns/utils/image/ImageEncoder.h>

class ImageValidator
{
public:
    static bool validate(brayns::Engine &engine, const std::string &filename)
    {
        auto &framebuffer = engine.getFrameBuffer();
        return validate(framebuffer, filename);
    }

    static bool validate(brayns::FrameBuffer &framebuffer,
                         const std::string &filename)
    {
        auto image = framebuffer.getImage();
        return validate(image, filename);
    }

    static bool validate(const brayns::Image &image,
                         const std::string &filename)
    {
        if (_shouldSaveTestImages())
        {
            brayns::ImageEncoder::save(image, filename);
        }
        auto reference = brayns::ImageDecoder::load(filename);
        auto same = validate(image, reference);
        if (!same)
        {
            std::cout << "Image does not match '" << filename << "'\n";
        }
    }

    static bool validate(const brayns::Image &image,
                         const brayns::Image &reference)
    {
        return image == reference;
    }

private:
    static bool _shouldSaveTestImages()
    {
        return std::getenv("BRAYNS_SAVE_TEST_IMAGES");
    }
};
