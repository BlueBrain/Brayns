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

#include "DeflectPlugin.h"

#include <brayns/Brayns.h>
#include <plugins/engines/common/Engine.h>
#include <brayns/common/camera/AbstractManipulator.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/parameters/ApplicationParameters.h>

#ifdef BRAYNS_USE_ZEROEQ
#  include "ZeroEQPlugin.h"
#endif

namespace
{
const float wheelFactor = 1.f / 40.f;
}

namespace brayns
{

#ifdef BRAYNS_USE_ZEROEQ
    DeflectPlugin::DeflectPlugin( Brayns& brayns, ZeroEQPlugin& zeroeq )
#else
    DeflectPlugin::DeflectPlugin( Brayns& brayns )
#endif
    : ExtensionPlugin( brayns )
{
    brayns.getKeyboardHandler().registerKeyboardShortcut(
        '*', "Enable/Disable Deflect streaming",
                [&] { _params.setEnabled( !_params.getEnabled( )); });

    // ensure streaming is off initially, otherwise our logic in run() won't work
    _params.setEnabled( false );

#ifdef BRAYNS_USE_ZEROEQ
    if( !zeroeq )
        return;

    zeroeq->handle( _params );

    // TODO needs proper exposure from ZeroEQ/ZeroBuf
    std::string schema = _params.getSchema();
    const std::string name = "Stream";
    schema.replace( schema.find( name ), name.length(), "StreamTo" );
    zeroeq->handlePUT( "lexis::render::StreamTo", schema,
                       std::bind( &::lexis::render::Stream::fromJSON,
                                    std::ref(_params), std::placeholders::_1 ));
#endif
}

void DeflectPlugin::run()
{
    if( _stream )
    {
        const bool changed = _stream->getId() != _params.getIdString() ||
                             _stream->getHost() != _params.getHostString();
        if( changed )
            _stream.reset();
    }

    if( _previousHost != _params.getHostString( ))
    {
        _params.setEnabled( true );
        _previousHost = _params.getHostString();
    }

    const bool deflectEnabled = _params.getEnabled();
    if( _stream && _stream->isConnected() && !deflectEnabled )
    {
        BRAYNS_INFO << "Closing Deflect stream" << std::endl;
        _stream.reset();
    }

    if( deflectEnabled && !_stream )
        _initializeDeflect();

    if( deflectEnabled && _stream && _stream->isConnected() )
    {
        _sendDeflectFrame();
        if( _handleDeflectEvents( ))
        {
            _brayns.getFrameBuffer().clear();
            _brayns.getRenderer().commit();
        }
    }
}

void DeflectPlugin::_initializeDeflect()
{
    try
    {
        _stream.reset( new deflect::Stream( _params.getIdString(),
                                            _params.getHostString(),
                                            _params.getPort( )));

        if( _stream->isConnected( ))
            BRAYNS_INFO << "Deflect successfully connected to Tide on host "
                        << _stream->getHost() << std::endl;
        else
            BRAYNS_ERROR << "Deflect failed to connect to Tide on host "
                         << _stream->getHost() << std::endl;

        if( !_stream->registerForEvents( ))
            BRAYNS_ERROR << "Deflect failed to register for events!" << std::endl;

        _params.setId( _stream->getId( ));
        _params.setHost( _stream->getHost( ));
    }
    catch( std::runtime_error& ex )
    {
        BRAYNS_ERROR << "Deflect failed to initialize. " << ex.what() << std::endl;
        _params.setEnabled( false );
        return;
    }
}

void DeflectPlugin::_sendDeflectFrame()
{
    FrameBuffer& frameBuffer = _brayns.getFrameBuffer();
    const Vector2i& frameSize = frameBuffer.getSize();
    void* data = frameBuffer.getColorBuffer();
    if( data )
        _send( frameSize, (unsigned long*)data, true);
}

bool DeflectPlugin::_handleDeflectEvents()
{
    if( !_stream->hasEvent( ))
        return false;

    while( _stream->hasEvent() )
    {
        const deflect::Event& event = _stream->getEvent();
        switch( event.type )
        {
        case deflect::Event::EVT_PRESS:
            _previousPos = _getWindowPos( event );
            _panOrPinch = false;
            break;
        case deflect::Event::EVT_MOVE:
        case deflect::Event::EVT_RELEASE:
        {
            const auto pos = _getWindowPos( event );
            if( !_panOrPinch )
                _brayns.getCameraManipulator().dragLeft( pos, _previousPos );
            _previousPos = pos;
            _panOrPinch = false;
            break;
        }
        case deflect::Event::EVT_PAN:
        {
            const auto pos = _getWindowPos( event );
            _brayns.getCameraManipulator().dragMiddle( pos, _previousPos );
            _previousPos = pos;
            _panOrPinch = true;
            break;
        }
        case deflect::Event::EVT_PINCH:
        {
            const auto pos = _getWindowPos( event );
            const auto delta = _getZoomDelta( event );
            _brayns.getCameraManipulator().wheel( pos, delta * wheelFactor );
            _panOrPinch = true;
            break;
        }
        case deflect::Event::EVT_KEY_PRESS:
        {
            _brayns.getKeyboardHandler().handleKeyboardShortcut( event.text[0] );
            break;
        }
        case deflect::Event::EVT_VIEW_SIZE_CHANGED:
        {
            _brayns.reshape( Vector2ui( event.dx, event.dy ));
            break;
        }
        case deflect::Event::EVT_CLOSE:
            _params.setEnabled( false );
            _params.setHost( "" );
            _previousHost.clear();
            _stream.reset();
            return true;
        default:
            break;
        }
    }

    return true;
}

void DeflectPlugin::_send(
    const Vector2i& windowSize,
    unsigned long* imageData,
    const bool swapYAxis )
{
    deflect::ImageWrapper deflectImage( imageData, windowSize.x(),
                                        windowSize.y(), deflect::RGBA );

    deflectImage.compressionQuality = _params.getQuality();
    deflectImage.compressionPolicy =
        _params.getCompression() ?
        deflect::COMPRESSION_ON : deflect::COMPRESSION_OFF;
    if( swapYAxis )
        deflect::ImageWrapper::swapYAxis( (void*)imageData, windowSize.x(),
                                          windowSize.y(), 4 );

    const bool success = _stream->send( deflectImage ) && _stream->finishFrame();
    if( !success )
    {
        if( !_stream->isConnected() )
            BRAYNS_INFO << "Stream closed, exiting." << std::endl;
        else
            BRAYNS_ERROR << "failure in deflectStreamSend()" << std::endl;
    }
}

Vector2d DeflectPlugin::_getWindowPos( const deflect::Event& event ) const
{
    const auto windowSize = _brayns.getFrameBuffer().getSize();
    return { event.mouseX * windowSize.x(), event.mouseY * windowSize.y() };
}

double DeflectPlugin::_getZoomDelta( const deflect::Event& pinchEvent ) const
{
    const auto windowSize = _brayns.getFrameBuffer().getSize();
    const auto dx = pinchEvent.dx * windowSize.x();
    const auto dy = pinchEvent.dy * windowSize.y();
    return std::copysign( std::sqrt( dx * dx + dy * dy ), dx + dy );
}

}
