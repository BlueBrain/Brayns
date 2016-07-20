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

#include <brayns/common/camera/Camera.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/common/scene/Scene.h>

#define BOOST_TEST_MODULE brayns
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( simple_construction )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    BOOST_CHECK_NO_THROW( brayns::Brayns( testSuite.argc,
                                 const_cast< const char** >( testSuite.argv )));
}

BOOST_AUTO_TEST_CASE( defaults )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    brayns::Brayns brayns( testSuite.argc,
                           const_cast< const char** >( testSuite.argv ));
    const auto& camera = brayns.getCamera();
    BOOST_CHECK_EQUAL( camera.getType(), brayns::CT_PERSPECTIVE );
    BOOST_CHECK_EQUAL( camera.getPosition(),
            brayns::Vector3f( 0, 0, std::numeric_limits< float >::infinity( )));
    BOOST_CHECK_EQUAL( camera.getTarget(), brayns::Vector3f( ));
    BOOST_CHECK_EQUAL( camera.getUpVector(), brayns::Vector3f( 0, 1, 0 ));
    BOOST_CHECK_EQUAL( camera.getAspectRatio(), 4.f/3.f );
    BOOST_CHECK_EQUAL( camera.getAperture(), 0.f );
    BOOST_CHECK_EQUAL( camera.getFocalLength(), 0.f );

    auto& fb = brayns.getFrameBuffer();
    BOOST_CHECK( !fb.getColorBuffer( ));
    BOOST_CHECK_EQUAL( fb.getColorDepth(), 4 );
    BOOST_CHECK( !fb.getDepthBuffer( ));
    BOOST_CHECK_EQUAL( fb.getSize(), brayns::Vector2i( 800, 600 ));

    auto& pm = brayns.getParametersManager();
    const auto& appParams = pm.getApplicationParameters();
    BOOST_CHECK_EQUAL( appParams.getWindowSize(), brayns::Vector2ui( 800, 600 ));
    BOOST_CHECK_EQUAL( appParams.getCamera(), "perspective" );
    BOOST_CHECK_EQUAL( appParams.getDeflectHostname(), "" );
    BOOST_CHECK_EQUAL( appParams.getDeflectStreamname(), "" );
    BOOST_CHECK( !appParams.isBenchmarking( ));
    BOOST_CHECK_EQUAL( appParams.getJpegCompression(), 100 );
    BOOST_CHECK_EQUAL( appParams.getJpegSize(), brayns::Vector2ui( 800, 600 ));

    const auto& renderParams = pm.getRenderingParameters();
    BOOST_CHECK_EQUAL( renderParams.getEngine(), "ospray" );
    BOOST_CHECK_EQUAL( renderParams.getModule(), "" );
    BOOST_CHECK_EQUAL( renderParams.getRenderer(), "exobj" );
    BOOST_CHECK_EQUAL( renderParams.getRenderers().size(), 3 );
    BOOST_CHECK( !renderParams.getShadows( ));
    BOOST_CHECK( !renderParams.getSoftShadows( ));
    BOOST_CHECK_EQUAL( renderParams.getAmbientOcclusionStrength(), 0.f );
    BOOST_CHECK_EQUAL( renderParams.getMaterialType(), brayns::MT_DIFFUSE );
    BOOST_CHECK_EQUAL( renderParams.getSamplesPerPixel(), 1 );
    BOOST_CHECK( !renderParams.getLightEmittingMaterials( ));
    BOOST_CHECK_EQUAL( renderParams.getBackgroundColor(),
                       brayns::Vector3f( 0, 0, 0 ));
    BOOST_CHECK_EQUAL( renderParams.getDetectionDistance(), 1.f );
    BOOST_CHECK( renderParams.getDetectionOnDifferentMaterial( ));
    BOOST_CHECK_EQUAL( renderParams.getDetectionNearColor(),
                       brayns::Vector3f( 1, 0, 0 ));
    BOOST_CHECK_EQUAL( renderParams.getDetectionFarColor(),
                       brayns::Vector3f( 0, 1, 0 ));
    BOOST_CHECK_EQUAL( renderParams.getEpsilon(), 1.e-3f );
    BOOST_CHECK_EQUAL( renderParams.getCameraType(), brayns::CT_PERSPECTIVE );
    BOOST_CHECK_EQUAL( renderParams.getHDRI(), "" );

    const auto& geomParams = pm.getGeometryParameters();
    BOOST_CHECK_EQUAL( geomParams.getMorphologyFolder(), "" );
    BOOST_CHECK_EQUAL( geomParams.getPDBFile(), "" );
    BOOST_CHECK_EQUAL( geomParams.getMeshFolder(), "" );
    BOOST_CHECK_EQUAL( geomParams.getCircuitConfiguration(), "" );
    BOOST_CHECK_EQUAL( geomParams.getLoadCacheFile(), "" );
    BOOST_CHECK_EQUAL( geomParams.getSaveCacheFile(), "" );
    BOOST_CHECK_EQUAL( geomParams.getTarget(), "" );
    BOOST_CHECK_EQUAL( geomParams.getReport(), "" );
    BOOST_CHECK_EQUAL( geomParams.getRadiusMultiplier(), 1.f );
    BOOST_CHECK_EQUAL( geomParams.getRadiusCorrection(), 0.f );
    BOOST_CHECK_EQUAL( geomParams.getColorScheme(), brayns::CS_NONE );
    BOOST_CHECK_EQUAL( geomParams.getSceneEnvironment(), brayns::SE_NONE );
    BOOST_CHECK_EQUAL( geomParams.getGeometryQuality(), brayns::GQ_MAX_QUALITY );
    BOOST_CHECK_EQUAL( geomParams.getMorphologySectionTypes(), brayns::MST_ALL );
    BOOST_CHECK_EQUAL( geomParams.getMorphologyLayout().type, brayns::ML_NONE );
    BOOST_CHECK_EQUAL( geomParams.getNonSimulatedCells(), 0 );
    BOOST_CHECK_EQUAL( geomParams.getLastSimulationFrame(), 0 );
    BOOST_CHECK_EQUAL( geomParams.getFirstSimulationFrame(), 0 );

    const auto& sceneParams = pm.getSceneParameters();
    BOOST_CHECK_EQUAL( sceneParams.getTimestamp(),
                       std::numeric_limits< size_t >::max( ));

    auto& scene = brayns.getScene();
    BOOST_CHECK( scene.getMaterial( 0 ));
    BOOST_CHECK_EQUAL( scene.getWorldBounds(), brayns::Boxf( ));
}

BOOST_AUTO_TEST_CASE( render_two_frames_and_compare_they_are_same )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    brayns::Brayns brayns( testSuite.argc,
                           const_cast< const char** >( testSuite.argv ));

    brayns.render();

    auto& fb = brayns.getFrameBuffer();
    const auto& size = fb.getSize();
    const size_t bytes = size[0] * size[1];
    std::vector< uint8_t > oldBuffer( bytes );
    fb.map();
    memcpy( oldBuffer.data(), fb.getColorBuffer(), bytes );
    fb.unmap();

    brayns.render();

    fb.map();
    BOOST_CHECK_EQUAL_COLLECTIONS( oldBuffer.begin(), oldBuffer.end(),
                                   fb.getColorBuffer(),
                                   fb.getColorBuffer()+bytes );
    fb.unmap();
}
