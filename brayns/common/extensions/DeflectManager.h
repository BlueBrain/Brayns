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

#ifndef DEFLECTMANAGER_H
#define DEFLECTMANAGER_H

#include <brayns/common/types.h>
#include <iostream>
#include <deflect/Stream.h>

namespace brayns
{

class BaseWindow;

/** Handles the bidirectional communication with the DisplayCluster server
 */
class DeflectManager
{

public:

    /** Constructs and initializes the connection to DisplayCluser server
     *
     * @param hostname name of the machine hosting DisplayCluster server
     * @param streamName arbitraty stream name for current application
     * @param compressionQuality percentage of quality of the compressed
     * image sent to DisplayCluster
     */
    DeflectManager(
            const std::string& hostname,
            const std::string& streamName,
            bool activateCompression,
            int compressionQuality);

    ~DeflectManager();

    /** Send an image to DisplayCluster
     *
     * @param imageSize size of the image
     * @param buffer containing the image
     * @param swapXAxis enables a horizontal flip operation on the image
     */
    void send(
            const ospray::vec2i imageSize,
            uint32 *imageData,
            bool swapXAxis);

    /** Handles touch events provided by DisplayCluster
     *
     * @param window window handling the event
     */
    void handleTouchEvents( BaseWindow* window );

    /** Handles touch events provided by DisplayCluster
     *
     * @param cameraPos new camera position
     */
    void handleTouchEvents( ospray::vec3f& cameraPos );

private:
    bool deflectInteraction_;
    bool deflectCompressImage_;
    unsigned int deflectCompressionQuality_;
    std::string deflectHostname_;
    std::string deflectStreamName_;
    deflect::Stream* deflectStream_;
};

}

#endif // DEFLECTMANAGER_H
