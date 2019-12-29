#pragma once

/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#include <brayns/Brayns.h>

#include <brayns/common/utils/imageUtils.h>
#include <brayns/engineapi/FrameBuffer.h>

#include <deps/perceptualdiff/metric.h>
#include <deps/perceptualdiff/rgba_image.h>

#include <tests/paths.h>

#include <iostream>

#ifdef BRAYNS_USE_NETWORKING
#include <ImageGenerator.h>
#include <brayns/common/utils/base64/base64.h>
#include <fstream>
#endif

inline std::unique_ptr<pdiff::RGBAImage> createPDiffRGBAImage(FIBITMAP* image)
{
    const auto w = FreeImage_GetWidth(image);
    const auto h = FreeImage_GetHeight(image);

    auto result = std::make_unique<pdiff::RGBAImage>(w, h, "");
    // Copy the image over to our internal format, FreeImage has the scanlines
    // bottom to top though.
    auto dest = result->get_data();
    for (unsigned int y = 0; y < h; y++, dest += w)
    {
        const auto scanline = reinterpret_cast<const unsigned int*>(
            FreeImage_GetScanLine(image, h - y - 1));
        memcpy(dest, scanline, sizeof(dest[0]) * w);
    }

    return result;
}

inline std::unique_ptr<pdiff::RGBAImage> createPDiffRGBAImage(
    brayns::FrameBuffer& fb)
{
    return createPDiffRGBAImage(FreeImage_ConvertTo32Bits(fb.getImage().get()));
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

#ifdef BRAYNS_USE_NETWORKING
inline bool compareBase64TestImage(
    const brayns::ImageGenerator::ImageBase64& image,
    const std::string& filename)
{
    auto decodedImage = base64_decode(image.data);

    static bool saveImages = getenv("BRAYNS_SAVE_TEST_IMAGES");
    if (saveImages)
    {
        std::fstream file(filename, std::ios::out);
        file << decodedImage;
    }

    auto freeImageMem =
        FreeImage_OpenMemory((BYTE*)decodedImage.data(), decodedImage.length());
    const auto fif = FreeImage_GetFileTypeFromMemory(freeImageMem, 0);
    auto decodedFreeImage = FreeImage_LoadFromMemory(fif, freeImageMem, 0);

    const auto testImage{createPDiffRGBAImage(decodedFreeImage)};
    auto imageDiff = clonePDiffRGBAImage(*testImage);

    auto result = _compareImage(*testImage, filename, *imageDiff);

    FreeImage_Unload(decodedFreeImage);
    FreeImage_CloseMemory(freeImageMem);

    return result;
}
#endif
