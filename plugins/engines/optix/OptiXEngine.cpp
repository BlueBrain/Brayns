/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "OptiXEngine.h"

#include <brayns/common/input/KeyboardHandler.h>

#include <plugins/engines/optix/OptiXRenderer.h>
#include <plugins/engines/optix/OptiXScene.h>
#include <plugins/engines/optix/OptiXFrameBuffer.h>
#include <plugins/engines/optix/OptiXCamera.h>
#include <plugins/engines/optix/OptiXUtils.h>

namespace brayns
{

OptiXEngine::OptiXEngine(
    int, const char **, ParametersManagerPtr parametersManager )
    : Engine()
    , _context( nullptr )
{
    BRAYNS_INFO << "Initializing OptiX" << std::endl;
    _initializeContext();

    BRAYNS_INFO << "Initializing renderers" << std::endl;
    _activeRenderer =
        parametersManager->getRenderingParameters().getRenderer();

    _rendererNames = parametersManager->getRenderingParameters().getRenderers();

    Renderers renderersForScene;
    for( std::string renderer: _rendererNames )
    {
        _renderers[renderer].reset(
            new OptiXRenderer( renderer, *parametersManager, _context ));
        renderersForScene.push_back( _renderers[renderer] );
    }

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene.reset( new OptiXScene( renderersForScene, *parametersManager, _context));

    _scene->setMaterials( MT_DEFAULT, NB_MAX_MATERIALS );

    BRAYNS_INFO << "Initializing frame buffer" << std::endl;
    _frameSize =
        parametersManager->getApplicationParameters( ).getWindowSize( );

    const bool accumulation = parametersManager->getApplicationParameters().getFilters().empty( );

    _frameBuffer.reset( new OptiXFrameBuffer(
        _frameSize, FBF_RGBA_I8, accumulation, _context ));
    _camera.reset( new OptiXCamera(
        parametersManager->getRenderingParameters().getCameraType(),
        _context));

    _keyboardHandler.reset( new KeyboardHandler( _scene, parametersManager ));

    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

OptiXEngine::~OptiXEngine()
{
    if( _context )
    {
        _context->destroy();
        _context = nullptr;
    }
}

void OptiXEngine::_initializeContext()
{
    // Set up context
    _context = optix::Context::create();
    if( !_context )
        BRAYNS_THROW( std::runtime_error( "Failed to initialize OptiX" ));

    _context->setRayTypeCount( 2 );
    _context->setEntryPointCount( 1 );
    _context->setStackSize( 4096 );

    unsigned int num_devices;
    unsigned int version;
    unsigned int i;

    rtDeviceGetDeviceCount(&num_devices);
    rtGetVersion(&version);

    BRAYNS_INFO << "Number of CUDA Devices: " + std::to_string( num_devices ) << std::endl;

    for( i = 0; i < num_devices; ++i )
    {
        char deviceName[256];
        int computeCaps[2];
        int clock_rate;

        RT_CHECK_ERROR( rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_NAME, sizeof( deviceName ), deviceName ));
        BRAYNS_INFO << "Device " + std::to_string( i ) + ": " +
            std::string( deviceName ) << std::endl;

        RT_CHECK_ERROR( rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY, sizeof( computeCaps ), &computeCaps ));
        BRAYNS_INFO << "- Compute Support: " + std::to_string( computeCaps[0] ) +
            std::to_string( computeCaps[1] ) << std::endl;

        RT_CHECK_ERROR( rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_TOTAL_MEMORY, sizeof( _totalMemory ), &_totalMemory ));
        BRAYNS_INFO << "- Total Memory: " + std::to_string( _totalMemory ) + " bytes ["
                    + std::to_string( _totalMemory / 1024 / 1024 ) + " MB]"
                    << std::endl;

        RT_CHECK_ERROR( rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_CLOCK_RATE, sizeof( clock_rate ), &clock_rate ));
        BRAYNS_INFO << "- Clock Rate: " + std::to_string( clock_rate / 1000 ) + " MHz" << std::endl;
    }
}

std::string OptiXEngine::name() const
{
    return "OptiX";
}

void OptiXEngine::commit()
{
    for( std::string renderer: _rendererNames )
    {
        _renderers[renderer]->setScene( _scene );
        _renderers[renderer]->setCamera( _camera );
        _renderers[renderer]->commit( );
    }
    _camera->commit( );
}

void OptiXEngine::render()
{
    if( _scene->getSimulationHandler() )
        _scene->commitSimulationData();

    if( _scene->getVolumeHandler() )
        _scene->commitVolumeData();

    _renderers[_activeRenderer]->render( _frameBuffer );
}

void OptiXEngine::preRender()
{
    _frameBuffer->map();
}

void OptiXEngine::postRender()
{
    _frameBuffer->unmap();
}

}
