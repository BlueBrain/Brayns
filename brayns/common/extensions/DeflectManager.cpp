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
    : deflectInteraction_(false),
      deflectCompressImage_(activateCompression),
      deflectCompressionQuality_(compressionQuality),
      deflectHostname_(hostname),
      deflectStreamName_(streamName),
      deflectStream_(NULL)
{
    BRAYNS_INFO << "Connecting to DisplayCluster on host " <<
                   deflectHostname_ << std::endl;

    deflectStream_ = new deflect::Stream(deflectStreamName_, deflectHostname_);
    if( !deflectStream_->isConnected())
    {
        BRAYNS_ERROR << "Could not connect to " <<
                        deflectHostname_ << std::endl;
        delete deflectStream_;
        deflectStream_ = NULL;
    }

    if( deflectStream_ && !deflectStream_->registerForEvents( ))
    {
        std::cerr << "Could not register for events!" << std::endl;
        delete deflectStream_;
        deflectStream_ = NULL;
    }
}

DeflectManager::~DeflectManager()
{
}

void DeflectManager::send(
    const ospray::vec2i windowSize,
    uint32 *imageData,
    const bool swapXAxis)
{
    if( !deflectStream_ ) return;

    deflect::ImageWrapper deflectImage(
                imageData, windowSize.x, windowSize.y, deflect::RGBA);

    deflectImage.compressionPolicy =
            deflectCompressImage_ ?
                deflect::COMPRESSION_ON : deflect::COMPRESSION_OFF;

    deflectImage.compressionQuality = deflectCompressionQuality_;
    if( swapXAxis )
        deflect::ImageWrapper::swapYAxis(
                    (void*)imageData, windowSize.x, windowSize.y, 4);

    bool success = deflectStream_->send(deflectImage);
    deflectStream_->finishFrame();

    if(!success)
    {
        if (!deflectStream_->isConnected())
            BRAYNS_ERROR << "Stream closed, exiting." << std::endl;
        else
            BRAYNS_ERROR << "failure in deflectStreamSend()" << std::endl;
    }
}

bool DeflectManager::handleTouchEvents(
    ospray::vec2f& position,
    ospray::vec2f& wheelDelta,
    bool &pressed,
    bool &closeApplication )
{
    if(!deflectStream_ || !deflectStream_->isRegisteredForEvents())
        return false;

    /* increment rotation angle according to interaction, or by a constant rate
     * if interaction is not enabled. Note that mouse position is in normalized
     * window coordinates: (0,0) to (1,1)
     * Note: there is a risk of missing events since we only process the
     * latest state available. For more advanced applications, event
     * processing should be done in a separate thread.
     */
    while (deflectStream_->hasEvent())
    {
        const deflect::Event& event = deflectStream_->getEvent();
        if (event.type == deflect::Event::EVT_CLOSE)
        {
            BRAYNS_INFO << "Received close..." << std::endl;
            closeApplication = true;
        }

        pressed =  (event.type == deflect::Event::EVT_PRESS);

        if (event.type == deflect::Event::EVT_WHEEL)
        {
            wheelDelta.x = event.dx;
            wheelDelta.y = event.dy;
        }
        position.x = event.mouseX;
        position.y = event.mouseY;
    }
    return true;
}

}
