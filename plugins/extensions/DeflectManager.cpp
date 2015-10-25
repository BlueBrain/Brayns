/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
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

#include <brayns/common/ui/BaseWindow.h>
#include <brayns/common/log.h>

#include "DeflectManager.h"

namespace brayns
{

DeflectManager::DeflectManager(
        const std::string& hostname,
        const std::string& streamName,
        const bool activateCompression,
        const int compressionQuality )
    : _interaction(false),
      _compressImage(activateCompression),
      _compressionQuality(compressionQuality),
      _hostname(hostname),
      _streamName(streamName),
      _stream(nullptr)
{
    BRAYNS_INFO << "Connecting to DisplayCluster on host " <<
                   _hostname << std::endl;

    _stream.reset(new deflect::Stream(_streamName, _hostname));
    if( !_stream->isConnected())
        BRAYNS_ERROR << "Could not connect to " << _hostname << std::endl;

    if( _stream && !_stream->registerForEvents( ))
        BRAYNS_ERROR << "Could not register for events!" << std::endl;
}

DeflectManager::~DeflectManager()
{
}

void DeflectManager::send(
    const Vector2i& windowSize,
    uint32 *imageData,
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
        deflect::ImageWrapper::swapYAxis((void*)imageData, windowSize.x(), windowSize.y(), 4);

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

bool DeflectManager::handleTouchEvents(
    Vector2f& position,
    Vector2f& wheelDelta,
    bool& pressed,
    bool& closeApplication )
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
            closeApplication = true;
        }

        pressed = (event.type == deflect::Event::EVT_PRESS);

        if (event.type == deflect::Event::EVT_WHEEL)
            wheelDelta = Vector2f(event.dx, event.dy);

        position = Vector2f(event.mouseX, event.mouseY);
    }
    return true;
}

}
