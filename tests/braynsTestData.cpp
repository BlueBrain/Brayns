
/* Copyright (c) 2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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
#include <tests/paths.h>

#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/utils/ImageUtils.h>

#define BOOST_TEST_MODULE braynsTestData
#include <boost/test/unit_test.hpp>

#include <perceptualdiff/metric.h>
#include <perceptualdiff/rgba_image.h>

//#define GENERATE_TESTDATA

#ifdef GENERATE_TESTDATA
void writeTestData(const std::string& filename, brayns::FrameBuffer& fb)
{
    auto image = brayns::freeimage::getImageFromFrameBuffer(fb);
    FreeImage_Save(FreeImage_GetFIFFromFilename(filename.c_str()), image.get(),
                   std::string(BRAYNS_TESTDATA + filename).c_str());
}
#endif

std::unique_ptr<pdiff::RGBAImage> createPDiffRGBAImage(brayns::FrameBuffer& fb)
{
    brayns::freeimage::ImagePtr image(FreeImage_ConvertTo32Bits(
        brayns::freeimage::getImageFromFrameBuffer(fb).get()));

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

    return std::move(result);
}

void compareTestData(const std::string& filename, brayns::FrameBuffer& fb)
{
    auto testImage = createPDiffRGBAImage(fb);
    const auto referenceImage{
        pdiff::read_from_file(BRAYNS_TESTDATA + filename)};
    BOOST_CHECK(pdiff::yee_compare(*referenceImage, *testImage));
}

BOOST_AUTO_TEST_CASE(render_two_frames_and_compare_they_are_same)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    auto& fb = brayns.getEngine().getFrameBuffer();
    const auto& size = fb.getSize();
    fb.setAccumulation(false);
    fb.resize(size);

    const uint16_t depth = fb.getColorDepth();
    const size_t bytes = size[0] * size[1] * depth;

    fb.clear();
    brayns.render();

    fb.map();
    pdiff::RGBAImage oldImage(size[0], size[1]);
    memcpy(oldImage.get_data(), fb.getColorBuffer(), bytes);
    fb.unmap();

    fb.clear();
    brayns.render();

    fb.map();
    pdiff::RGBAImage newImage(size[0], size[1]);
    memcpy(newImage.get_data(), fb.getColorBuffer(), bytes);
    BOOST_CHECK(pdiff::yee_compare(oldImage, newImage));
    fb.unmap();
}

BOOST_AUTO_TEST_CASE(render_xyz_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    auto path = BRAYNS_TESTDATA + std::string("files/monkey.xyz");
    const char* argv[] = {app, path.c_str(), "--accumulation", "off"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData("testdataMonkey.png", brayns.getEngine().getFrameBuffer());
#endif
    compareTestData("testdataMonkey.png", brayns.getEngine().getFrameBuffer());
}

#ifdef BRAYNS_USE_BBPTESTDATA
BOOST_AUTO_TEST_CASE(render_circuit_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--accumulation",
                          "off",
                          "--circuit-targets",
                          "Layer1",
                          "--samples-per-pixel",
                          "16"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData("testdataLayer1.png", brayns.getEngine().getFrameBuffer());
#endif
    compareTestData("testdataLayer1.png", brayns.getEngine().getFrameBuffer());
}

BOOST_AUTO_TEST_CASE(render_sdf_circuit_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--accumulation",
                          "off",
                          "--circuit-targets",
                          "Layer1",
                          "--morphology-dampen-branch-thickness-changerate",
                          "true",
                          "--morphology-use-sdf-geometries",
                          "true",
                          "--samples-per-pixel",
                          "16"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData("testSdfCircuit.png", brayns.getEngine().getFrameBuffer());
#endif
    compareTestData("testSdfCircuit.png", brayns.getEngine().getFrameBuffer());
}
#endif

BOOST_AUTO_TEST_CASE(render_protein_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const std::string pdbFile(BRAYNS_TESTDATA + std::string("1bna.pdb"));
    const char* argv[] = {app, pdbFile.c_str(), "--accumulation", "off"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData("testdataProtein.png", brayns.getEngine().getFrameBuffer());
#endif
    compareTestData("testdataProtein.png", brayns.getEngine().getFrameBuffer());
}

BOOST_AUTO_TEST_CASE(render_protein_in_stereo_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const std::string pdbFile(BRAYNS_TESTDATA + std::string("1bna.pdb"));
    const char* argv[] = {app,   pdbFile.c_str(), "--accumulation",
                          "off", "--stereo-mode", "side-by-side"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData("testdataProteinStereo.png",
                  brayns.getEngine().getFrameBuffer());
#endif
    compareTestData("testdataProteinStereo.png",
                    brayns.getEngine().getFrameBuffer());
}
