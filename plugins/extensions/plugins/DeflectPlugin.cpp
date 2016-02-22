/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include <brayns/common/scene/Scene.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/camera/Camera.h>

namespace
{
    const size_t DEFAULT_COMPRESSION_QUALITY = 100;
}

namespace brayns
{

DeflectPlugin::DeflectPlugin(
    ApplicationParameters& applicationParameters,
    ExtensionParameters& extensionParameters )
    : ExtensionPlugin( applicationParameters, extensionParameters )
    , _alpha( 0 )
    , _theta( 0 )
    , _previousCameraPosition( 0.f, 0.f, 0.f )
    , _interaction(false)
    , _compressImage( DEFAULT_COMPRESSION_QUALITY != 0 )
    , _compressionQuality( DEFAULT_COMPRESSION_QUALITY )
    , _hostname( _applicationParameters.getDeflectHostname( ))
    , _streamName( _applicationParameters.getDeflectStreamname( ))
    , _stream( nullptr )

{
    _previousCameraPosition.z( ) =
        -_extensionParameters.scene->getWorldBounds( ).getSize().z( );
    _initializeDeflect( );
}

void DeflectPlugin::run()
{
    if( _stream->isConnected( ))
    {
        _sendDeflectFrame( );
        _handleDeflectEvents( );
    }
}

void DeflectPlugin::_initializeDeflect()
{
    BRAYNS_INFO << "Connecting to DisplayCluster on host " <<
                   _hostname << std::endl;

    _stream.reset(new deflect::Stream(_streamName, _hostname));
    if( !_stream->isConnected())
        BRAYNS_ERROR << "Could not connect to " << _hostname << std::endl;

    if( _stream && !_stream->registerForEvents( ))
        BRAYNS_ERROR << "Could not register for events!" << std::endl;
}

void DeflectPlugin::_sendDeflectFrame()
{
    const Vector2i frameSize = _extensionParameters.frameBuffer->getSize();
    _send( frameSize, (unsigned long*)
        _extensionParameters.frameBuffer->getColorBuffer(), false);
}

void DeflectPlugin::_handleDeflectEvents()
{
    const Vector2i frameSize = _extensionParameters.frameBuffer->getSize();

    HandledEvents handledEvents(
        Vector2f( 0.f, 0.f ),
        Vector2f( 0.f, 0.f ),
        false,
        false );

    if( _handleTouchEvents( handledEvents ))
    {
        if( handledEvents.pressed )
            _previousCameraPosition = Vector3f(
                handledEvents.position.x(), handledEvents.position.y(), 0.f);
        else
        {
            if( handledEvents.position.length() >
                std::numeric_limits<float>::epsilon() ||
                handledEvents.wheelDelta.y() >
                std::numeric_limits<float>::epsilon() )
            {
                const Vector3f& center = _extensionParameters.scene->
                    getWorldBounds().getCenter();
                const Vector3f& size = _extensionParameters.scene->
                    getWorldBounds().getSize();

                const float dx =
                    _previousCameraPosition.x() - handledEvents.position.x();
                const float dy =
                    _previousCameraPosition.y() - handledEvents.position.y();

                if(std::abs(dx)<=1.f)
                    _theta -= frameSize.x()/100.f*std::asin(dx);
                if(std::abs(dy)<=1.f)
                    _alpha += frameSize.y()/100.f*std::asin(dy);

                _previousCameraPosition.x() = handledEvents.position.x();
                _previousCameraPosition.y() = handledEvents.position.y();
                _previousCameraPosition.z() -= handledEvents.wheelDelta.y()*5.f;

                if(dx!=0.f || dy!=0.f || handledEvents.wheelDelta.y()!=0.f)
                {
                    Vector3f cameraPosition = size*Vector3f(
                        _previousCameraPosition.z( ) * std::cos( _alpha ) *
                            std::cos( _theta ),
                        _previousCameraPosition.z( ) * std::sin( _alpha ) *
                            std::cos( _theta ),
                        _previousCameraPosition.z( ) * std::sin( _theta ));

                    BRAYNS_INFO << "[" << _alpha << ", " << _theta
                        << "] " << cameraPosition << std::endl;

                    _extensionParameters.camera->set(
                        center + cameraPosition,
                        center,
                        _extensionParameters.camera->getUpVector());

                   _extensionParameters.frameBuffer->clear();
                }
            }
        }
    }
}

void DeflectPlugin::_send(
    const Vector2i& windowSize,
    unsigned long* imageData,
    const bool swapXAxis)
{
    if(!_stream->isConnected())
        return;

    deflect::ImageWrapper deflectImage(
                imageData, windowSize.x(), windowSize.y(), deflect::RGBA);

    deflectImage.compressionPolicy =
            _compressImage ? deflect::COMPRESSION_ON : deflect::COMPRESSION_OFF;

    deflectImage.compressionQuality = _compressionQuality;
    if( swapXAxis )
        deflect::ImageWrapper::swapYAxis(
            (void*)imageData, windowSize.x(), windowSize.y(), 4);

    bool success = _stream->send(deflectImage);
    _stream->finishFrame();

    if(!success)
    {
        if (!_stream->isConnected())
            BRAYNS_ERROR << "Stream closed, exiting." << std::endl;
        else
            BRAYNS_ERROR << "failure in deflectStreamSend()" << std::endl;
    }
}

bool DeflectPlugin::_handleTouchEvents( HandledEvents& handledEvents )
{
    if(!_stream || !_stream->isRegisteredForEvents())
        return false;

    /* increment rotation angle according to interaction, or by a constant rate
     * if interaction is not enabled. Note that mouse position is in normalized
     * window coordinates: (0,0) to (1,1)
     * Note: there is a risk of missing events since we only process the
     * latest state available. For more advanced applications, event
     * processing should be done in a separate thread.
     */
    while(_stream->hasEvent())
    {
        const deflect::Event& event = _stream->getEvent();
        if(event.type == deflect::Event::EVT_CLOSE)
        {
            BRAYNS_INFO << "Received close..." << std::endl;
            handledEvents.closeApplication = true;
        }

        handledEvents.pressed = (event.type == deflect::Event::EVT_PRESS);

        if (event.type == deflect::Event::EVT_WHEEL)
            handledEvents.wheelDelta = Vector2f(event.dx, event.dy);

        handledEvents.position = Vector2f(event.mouseX, event.mouseY);
    }
    return true;
}

}
