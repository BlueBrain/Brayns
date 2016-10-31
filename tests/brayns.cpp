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

#include <chrono>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

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
    BOOST_CHECK_EQUAL( camera.getPosition(), brayns::Vector3f( 0.5f, 0.5f, -0.5f ));
    BOOST_CHECK_EQUAL( camera.getTarget(), brayns::Vector3f( 0.5f, 0.5f, 0.5f ));
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
    BOOST_CHECK_EQUAL( appParams.getDeflectHostName(), "" );
    BOOST_CHECK_EQUAL( appParams.getDeflectStreamName(), "" );
    BOOST_CHECK_EQUAL( appParams.getDeflectEnabled(), true );
    BOOST_CHECK( !appParams.isBenchmarking( ));
    BOOST_CHECK_EQUAL( appParams.getJpegCompression(), 100 );
    BOOST_CHECK_EQUAL( appParams.getJpegSize(), brayns::Vector2ui( 800, 600 ));

    const auto& renderParams = pm.getRenderingParameters();
    BOOST_CHECK_EQUAL( renderParams.getEngine(), "ospray" );
    BOOST_CHECK_EQUAL( renderParams.getModule(), "" );
    BOOST_CHECK_EQUAL( renderParams.getRenderer(), "exobj" );
    BOOST_CHECK_EQUAL( renderParams.getRenderers().size(), 4 );
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
    BOOST_CHECK_EQUAL( geomParams.getStartSimulationTime(), 0.f );
    BOOST_CHECK_EQUAL( geomParams.getEndSimulationTime(), std::numeric_limits< float >::max() );
    BOOST_CHECK_EQUAL( geomParams.getSimulationValuesRange().x(),
                       std::numeric_limits< float >::max() );
    BOOST_CHECK_EQUAL( geomParams.getSimulationValuesRange().y(),
                       std::numeric_limits< float >::min() );

    const auto& sceneParams = pm.getSceneParameters();
    BOOST_CHECK_EQUAL( sceneParams.getTimestamp(), std::numeric_limits< float >::max( ));

    const auto& volumeParams = pm.getVolumeParameters();
    BOOST_CHECK_EQUAL( volumeParams.getDimensions(), brayns::Vector3ui( 0, 0, 0 ));
    BOOST_CHECK_EQUAL( volumeParams.getElementSpacing(), brayns::Vector3f( 1.f, 1.f, 1.f ));
    BOOST_CHECK_EQUAL( volumeParams.getOffset(), brayns::Vector3f( 0.f, 0.f, 0.f ));
    BOOST_CHECK_EQUAL( volumeParams.getSamplesPerRay(), 128 );

    auto& scene = brayns.getScene();
    BOOST_CHECK( scene.getMaterial( 0 ));

    brayns::Boxf defaultBoundingBox;
    defaultBoundingBox.merge( brayns::Vector3f( 0, 0, 0 ));
    defaultBoundingBox.merge( brayns::Vector3f( 1, 1, 1 ));
    BOOST_CHECK_EQUAL( scene.getWorldBounds(), defaultBoundingBox );
}

BOOST_AUTO_TEST_CASE( render_two_frames_and_compare_they_are_same )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    brayns::Brayns brayns( testSuite.argc,
                           const_cast< const char** >( testSuite.argv ));

    auto& fb = brayns.getFrameBuffer();
    const auto& size = fb.getSize();
    fb.setAccumulation( false );
    fb.resize( size );

    uint16_t depth = fb.getColorDepth();
    const size_t bytes = size[0] * size[1] * depth;
    std::vector< uint8_t > oldBuffer( bytes );

    fb.clear();
    brayns.render();

    fb.map();
    memcpy( oldBuffer.data(), fb.getColorBuffer(), bytes );
    fb.unmap();

    fb.clear();
    brayns.render();

    fb.map();
    BOOST_CHECK_EQUAL_COLLECTIONS( oldBuffer.begin(), oldBuffer.end(),
                                   fb.getColorBuffer(),
                                   fb.getColorBuffer() + bytes );
    fb.unmap();
}

#ifdef NDEBUG
BOOST_AUTO_TEST_CASE( default_scene_benckmark )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    brayns::Brayns brayns( testSuite.argc,
                           const_cast< const char** >( testSuite.argv ));

    high_resolution_clock::time_point startTime;
    uint64_t reference, shadows, softShadows, ambientOcclusion, allOptions;

    // Set default rendering parameters
    brayns::ParametersManager& params = brayns.getParametersManager();
    params.getRenderingParameters().setSamplesPerPixel(32);
    brayns.commit();

    // Start timer
    startTime = high_resolution_clock::now();
    brayns.render();
    reference = duration_cast< milliseconds >(
        high_resolution_clock::now() - startTime ).count();

    // Shadows
    params.getRenderingParameters().setShadows( true );
    brayns.commit();

    startTime = high_resolution_clock::now();
    brayns.render();
    shadows = duration_cast< milliseconds >(
        high_resolution_clock::now() - startTime ).count();

    // Shadows
    float t = float(shadows) / float(reference);
    BOOST_TEST_MESSAGE( "Shadows cost. expected: 165%, realized: " << t * 100.f );
    BOOST_CHECK( t < 1.65f );

    params.getRenderingParameters().setSoftShadows( true );
    brayns.commit();

    startTime = high_resolution_clock::now();
    brayns.render();
    softShadows = duration_cast< milliseconds >(
        high_resolution_clock::now() - startTime ).count();

    // Soft shadows
    t = float(softShadows) / float(reference);
    BOOST_TEST_MESSAGE( "Soft shadows cost. expected: 185%, realized: " << t * 100.f );
    BOOST_CHECK( t < 1.85f );

    // Ambient occlustion
    params.getRenderingParameters().setShadows( false );
    params.getRenderingParameters().setSoftShadows( false );
    params.getRenderingParameters().setAmbientOcclusionStrength( 1.f );
    brayns.commit();

    startTime = high_resolution_clock::now();
    brayns.render();
    ambientOcclusion = duration_cast< milliseconds >(
        high_resolution_clock::now() - startTime ).count();

    // Ambient occlusion
    t = float(ambientOcclusion) / float(reference);
    BOOST_TEST_MESSAGE( "Ambient occlusion cost. expected: 250%, realized: " << t * 100.f );
    BOOST_CHECK( t < 2.5f );

    // All options
    params.getRenderingParameters().setShadows( true );
    params.getRenderingParameters().setSoftShadows( true );
    params.getRenderingParameters().setAmbientOcclusionStrength( 1.f );
    brayns.commit();

    startTime = high_resolution_clock::now();
    brayns.render();
    allOptions = duration_cast< milliseconds >(
        high_resolution_clock::now() - startTime ).count();

    // All options
    t = float(allOptions) / float(reference);
    BOOST_TEST_MESSAGE( "All options cost. expected: 350%, realized: " << t * 100.f );
    BOOST_CHECK( t < 3.5f );
}
#endif

BOOST_AUTO_TEST_CASE( test_transfer_function )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    brayns::Brayns brayns( testSuite.argc,
                           const_cast< const char** >( testSuite.argv ));

    brayns::Scene& scene = brayns.getScene();
    brayns::TransferFunction& transferFunction = scene.getTransferFunction();

    // Red attribute
    brayns::Vector2fs& redControlPoints = transferFunction.getControlPoints( brayns::TF_RED );
    redControlPoints.push_back( brayns::Vector2f( 0.5f, 0.2f ) );
    redControlPoints.push_back( brayns::Vector2f( 0.6f, 1.0f ) );
    redControlPoints.push_back( brayns::Vector2f( 2.0f, 1.0f ) );

    // Green attribute
    brayns::Vector2fs& greenControlPoints = transferFunction.getControlPoints( brayns::TF_GREEN );
    greenControlPoints.push_back( brayns::Vector2f( 0.1f, 0.3f ) );
    greenControlPoints.push_back( brayns::Vector2f( 1.5f, 1.0f ) );

    // Blue attribute
    brayns::Vector2fs& blueControlPoints = transferFunction.getControlPoints( brayns::TF_BLUE );
    blueControlPoints.push_back( brayns::Vector2f( 0.0f, 0.2f ) );
    blueControlPoints.push_back( brayns::Vector2f( 0.5f, 0.0f ) );
    blueControlPoints.push_back( brayns::Vector2f( 2.0f, 0.0f ) );

    // Alpha attribute
    brayns::Vector2fs& alphaControlPoints = transferFunction.getControlPoints( brayns::TF_ALPHA );
    alphaControlPoints.push_back( brayns::Vector2f( 0.0f, 1.0f ) );

    // Emission attribute
    brayns::Vector2fs& emissionControlPoints = transferFunction.getControlPoints( brayns::TF_EMISSION );
    emissionControlPoints.push_back( brayns::Vector2f( 0.0f, 1.0f ) );
    emissionControlPoints.push_back( brayns::Vector2f( 2.0f, 0.0f ) );

    // Generate color map with 10 values
    transferFunction.resample(10);

    // Check diffuse color values
    const float precision = 1e-5f;
    const brayns::Vector4f expectedDiffuseColors[10] =
    {
        { 0.2f, 0.3f, 0.11111f, 1.f },
        { 0.2f, 0.41111, 0.02222f, 1.f },
        { 0.2f, 0.52222, 0.f, 1.f },
        { 1.f, 0.63333, 0.f, 1.f },
        { 1.f, 0.74444, 0.f, 1.f },
        { 1.f, 0.85555, 0.f, 1.f },
        { 1.f, 0.96666, 0.f, 1.f },
        { 1.f, 1.f, 0.f, 1.f },
        { 1.f, 1.f, 0.f, 1.f },
        { 1.f, 1.f, 0.f, 1.f }
    };

    brayns::Vector4fs& diffuseColors = transferFunction.getDiffuseColors();
    for( size_t i = 0; i < diffuseColors.size(); ++i )
    {
        BOOST_CHECK( fabs(diffuseColors[i].x() - expectedDiffuseColors[i].x()) < precision );
        BOOST_CHECK( fabs(diffuseColors[i].y() - expectedDiffuseColors[i].y()) < precision );
        BOOST_CHECK( fabs(diffuseColors[i].z() - expectedDiffuseColors[i].z()) < precision );
        BOOST_CHECK( fabs(diffuseColors[i].w() - expectedDiffuseColors[i].w()) < precision );
    }

    // Check emission intensity values
    const float expectedEmissionIntensities[10] =
        { 0.88888f, 0.77777f, 0.66666f, 0.55555, 0.44444f, 0.33333f, 0.22222f, 0.11111f, 0.f, 0.f };

    brayns::floats& emissionIntensities = transferFunction.getEmissionIntensities();
    for( size_t i = 0; i < emissionIntensities.size(); ++i )
        BOOST_CHECK( fabs(emissionIntensities[i] - expectedEmissionIntensities[i]) < precision );
}
