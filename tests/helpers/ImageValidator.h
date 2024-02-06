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

#pragma once

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <tests/paths.h>

#include <brayns/engine/core/Engine.h>
#include <brayns/utils/image/Image.h>
#include <brayns/utils/image/ImageDecoder.h>
#include <brayns/utils/image/ImageEncoder.h>

class ImageValidator
{
public:
    static bool validate(brayns::Engine &engine, const std::string &filename)
    {
        auto &framebuffer = engine.getFramebuffer();
        return validate(framebuffer, filename);
    }

    static bool validate(brayns::Framebuffer &framebuffer, const std::string &filename)
    {
        auto image = framebuffer.getImage();
        return validate(image, filename);
    }

    static bool validate(const brayns::Image &image, const std::string &filename)
    {
        std::cout << "Validation of image '" << filename << "'.\n";
        auto path = TestPaths::ReferenceImages::folder + filename;
        _saveIfNeeded(image, path);
        auto reference = brayns::ImageDecoder::load(path);
        return validate(image, reference);
    }

    static bool validate(const brayns::Image &image, const brayns::Image &reference)
    {
        auto evaluation = _evaluate(image, reference);
        std::cout << "Evaluation result: " << evaluation << ".\n";
        std::cout << "Evaluation threshold: " << _threshold << ".\n";
        return evaluation < _threshold;
    }

private:
    static constexpr double _threshold = 1.0;

    static void _saveIfNeeded(const brayns::Image &image, const std::string &path)
    {
        if (_isSavePngEnabled())
        {
            brayns::ImageEncoder::save(image, path);
        }
        if (_isSaveRawEnabled())
        {
            _saveRaw(image, path);
        }
    }

    static bool _isSavePngEnabled()
    {
        return std::getenv("BRAYNS_TEST_SAVE_PNG");
    }

    static bool _isSaveRawEnabled()
    {
        return std::getenv("BRAYNS_TEST_SAVE_RAW");
    }

    static void _saveRaw(const brayns::Image &image, const std::string &path)
    {
        std::filesystem::path modifiedPath(path);
        modifiedPath.replace_extension(".txt");
        std::ofstream stream(modifiedPath);
        auto data = static_cast<const char *>(image.getData());
        auto size = image.getSize();
        stream.write(data, size);
    }

    static double _evaluate(const brayns::Image &image, const brayns::Image &reference)
    {
        auto imageSize = image.getSize();
        auto referenceSize = reference.getSize();
        if (imageSize != referenceSize)
        {
            return false;
        }
        auto imageData = static_cast<const uint8_t *>(image.getData());
        auto referenceData = static_cast<const uint8_t *>(reference.getData());
        size_t errorSum = 0;
        for (size_t i = 0; i < imageSize; ++i)
        {
            errorSum += std::abs(int(imageData[i]) - int(referenceData[i]));
        }
        return double(errorSum) / double(imageSize);
    }
};
