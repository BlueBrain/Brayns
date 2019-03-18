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
#include <brayns/engine/FrameBuffer.h>

#include <deps/perceptualdiff/metric.h>
#include <deps/perceptualdiff/rgba_image.h>

#include <tests/paths.h>

#ifdef BRAYNS_USE_NETWORKING
#include <ImageGenerator.h>
#include <boost/filesystem.hpp>
#include <brayns/common/utils/base64/base64.h>
#include <fstream>
namespace fs = boost::filesystem;
#endif

inline std::unique_ptr<pdiff::RGBAImage> createPDiffRGBAImage(
    brayns::FrameBuffer& fb)
{
    brayns::freeimage::ImagePtr image(
        FreeImage_ConvertTo32Bits(fb.getImage().get()));

    const auto w = FreeImage_GetWidth(image.get());
    const auto h = FreeImage_GetHeight(image.get());

    auto result = std::make_unique<pdiff::RGBAImage>(w, h, "");
    // Copy the image over to our internal format, FreeImage has the scanlines
    // bottom to top though.
    auto dest = result->get_data();
    for (unsigned int y = 0; y < h; y++, dest += w)
    {
        const auto scanline = reinterpret_cast<const unsigned int*>(
            FreeImage_GetScanLine(image.get(), h - y - 1));
        memcpy(dest, scanline, sizeof(dest[0]) * w);
    }

    return result;
}

inline bool _compareImage(const pdiff::RGBAImage& image,
                          const std::string& filename,
                          const pdiff::PerceptualDiffParameters& parameters =
                              pdiff::PerceptualDiffParameters())
{
    const auto fullPath = std::string(BRAYNS_TESTDATA_IMAGES_PATH) + filename;
    const auto referenceImage{pdiff::read_from_file(fullPath)};
    return pdiff::yee_compare(*referenceImage, image, parameters);
}

inline bool compareTestImage(const std::string& filename,
                             brayns::FrameBuffer& fb,
                             const pdiff::PerceptualDiffParameters& parameters =
                                 pdiff::PerceptualDiffParameters())
{
    static bool saveImages = getenv("BRAYNS_SAVE_TEST_IMAGES");
    if (saveImages)
    {
        auto image = fb.getImage();
        FreeImage_Save(FreeImage_GetFIFFromFilename(filename.c_str()),
                       image.get(), filename.c_str());
    }

    auto testImage = createPDiffRGBAImage(fb);
    return _compareImage(*testImage, filename, parameters);
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

    const std::string newFilename(
        (fs::temp_directory_path() / fs::unique_path()).string());
    {
        std::fstream file(newFilename, std::ios::out);
        file << decodedImage;
    }
    const auto testImage{pdiff::read_from_file(newFilename)};
    return _compareImage(*testImage, filename);
}
#endif
