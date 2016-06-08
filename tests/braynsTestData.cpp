/* Copyright (c) 2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
 *
 * This file is part of BRayns
 */

#include <tests/paths.h>
#include <brayns/Brayns.h>

#include <brayns/common/renderer/FrameBuffer.h>

#define BOOST_TEST_MODULE braynsTestData
#include <boost/test/unit_test.hpp>

#include <lunchbox/memoryMap.h>

//#define GENERATE_TESTDATA

#ifdef GENERATE_TESTDATA
void writeTestData( const std::string& filename, brayns::FrameBuffer& fb )
{
    fb.map();
    const auto& size = fb.getSize();
    const size_t bytes = size[0] * size[1] * fb.getColorDepth();
    lunchbox::MemoryMap file( BRAYNS_TESTDATA + filename, bytes );
    memcpy( file.getAddress(), fb.getColorBuffer(), bytes );
    fb.unmap();
}
#endif

void compareTestData( const std::string& filename, brayns::FrameBuffer& fb )
{
    const lunchbox::MemoryMap file( BRAYNS_TESTDATA + filename );
    fb.map();
    const auto& size = fb.getSize();
    BOOST_CHECK_EQUAL( memcmp( file.getAddress(), fb.getColorBuffer(),
                               size[0] * size[1] * fb.getColorDepth( )), 0 );
    fb.unmap();
}

void checkFiles( const std::string& filenameA, const std::string& filenameB, bool same )
{
    const lunchbox::MemoryMap fileA( BRAYNS_TESTDATA + filenameA );
    const lunchbox::MemoryMap fileB( BRAYNS_TESTDATA + filenameB );

    BOOST_REQUIRE_EQUAL( fileA.getSize(), fileB.getSize( ));
    if( same )
        BOOST_CHECK_EQUAL( memcmp( fileA.getAddress(), fileB.getAddress(),
                                   fileA.getSize( )), 0 );
    else
        BOOST_CHECK_NE( memcmp( fileA.getAddress(), fileB.getAddress(),
                                fileA.getSize( )), 0 );
}

#ifdef BRAYNS_USE_BBPTESTDATA
BOOST_AUTO_TEST_CASE( render_circuit_and_compare )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = { app, "--circuit-config", BBP_TEST_BLUECONFIG3,
                           "--target", "Layer1", "--last-simulation-frame", "1" };
    const int argc = sizeof(argv)/sizeof(char*);

    brayns::Brayns brayns( argc, argv );
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData( "testdataLayer1.bin", brayns.getFrameBuffer( ));
#endif
    compareTestData( "testdataLayer1.bin", brayns.getFrameBuffer( ));
}
#endif

BOOST_AUTO_TEST_CASE( render_protein_and_compare )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const std::string pdbFile( BRAYNS_TESTDATA + std::string( "1bna.pdb" ));
    const char* argv[] = { app, "--pdb-file", pdbFile.c_str() };
    const int argc = sizeof(argv)/sizeof(char*);

    brayns::Brayns brayns( argc, argv );
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData( "testdataProtein.bin", brayns.getFrameBuffer( ));
#endif
    compareTestData( "testdataProtein.bin", brayns.getFrameBuffer( ));
}

BOOST_AUTO_TEST_CASE( render_protein_in_stereo_and_compare )
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const std::string pdbFile( BRAYNS_TESTDATA + std::string( "1bna.pdb" ));
    const char* argv[] = { app, "--pdb-file", pdbFile.c_str(),
                           "--camera-type", "2" };
    const int argc = sizeof(argv)/sizeof(char*);

    brayns::Brayns brayns( argc, argv );
    brayns.render();
#ifdef GENERATE_TESTDATA
    writeTestData( "testdataProteinStereo.bin", brayns.getFrameBuffer( ));
#endif
    compareTestData( "testdataProteinStereo.bin", brayns.getFrameBuffer( ));
    checkFiles( "testdataProtein.bin", "testdataProteinStereo.bin", false );
}
