/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include <brayns/Brayns.h>

#include <brayns/engine/FrameBuffer.h>
#include <brayns/utils/base64/base64.h>
#include <brayns/utils/image/Image.h>
#include <brayns/utils/image/ImageConverter.h>

#include <deps/perceptualdiff/metric.h>
#include <deps/perceptualdiff/rgba_image.h>

#include <tests/paths.h>

#include <fstream>
#include <iostream>

inline std::unique_ptr<pdiff::RGBAImage> createPDiffRGBAImage(
    const brayns::Image& image)
{
    auto width = image.getWidth();
    auto height = image.getHeight();

    auto result = std::make_unique<pdiff::RGBAImage>(width, height, "");

    auto destination = result->get_data();
    auto source = image.getData();
    auto size = image.getSize();

    std::memcpy(destination, source, image.getSize());

    return result;
}

inline std::unique_ptr<pdiff::RGBAImage> createPDiffRGBAImage(
    brayns::FrameBuffer& framebuffer)
{
    auto image = framebuffer.getImage();
    auto rgba = brayns::ImageConverter::convertToRgba(image);
    return createPDiffRGBAImage(rgba);
}

inline std::unique_ptr<pdiff::RGBAImage> clonePDiffRGBAImage(
    const pdiff::RGBAImage& image)
{
    auto result = std::make_unique<pdiff::RGBAImage>(image.get_width(),
                                                     image.get_height(), "");
    const auto dataSize = image.get_width() * image.get_height() * 4;
    memcpy(result->get_data(), image.get_data(), dataSize);
    return result;
}

inline bool _compareImage(const pdiff::RGBAImage& image,
                          const std::string& filename,
                          pdiff::RGBAImage& imageDiff,
                          const pdiff::PerceptualDiffParameters& parameters =
                              pdiff::PerceptualDiffParameters())
{
    const auto fullPath = std::string(BRAYNS_TESTDATA_IMAGES_PATH) + filename;
    const auto referenceImage{pdiff::read_from_file(fullPath)};
    std::string errorOutput;
    bool success =
        pdiff::yee_compare(*referenceImage, image, parameters, nullptr, nullptr,
                           &errorOutput, &imageDiff, nullptr);
    if (!success)
        std::cerr << "Pdiff failure reason: " << errorOutput;
    return success;
}

inline bool compareTestImage(const std::string& filename,
                             brayns::FrameBuffer& fb,
                             const pdiff::PerceptualDiffParameters& parameters =
                                 pdiff::PerceptualDiffParameters())
{
    static bool saveTestImages = getenv("BRAYNS_SAVE_TEST_IMAGES");
    static bool saveDiffImages = getenv("BRAYNS_SAVE_DIFF_IMAGES");
    if (saveTestImages)
        createPDiffRGBAImage(fb)->write_to_file(filename);

    auto testImage = createPDiffRGBAImage(fb);
    auto imageDiff = clonePDiffRGBAImage(*testImage);

    bool success = _compareImage(*testImage, filename, *imageDiff, parameters);

    if (!success && saveDiffImages)
    {
        const auto filenameDiff = ("diff_" + filename);
        imageDiff->write_to_file(filenameDiff);
    }

    return success;
}
