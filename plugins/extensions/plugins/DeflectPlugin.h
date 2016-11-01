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

#ifndef DEFLECTPLUGIN_H
#define DEFLECTPLUGIN_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <deflect/Stream.h>

namespace brayns
{

class DeflectPlugin : public ExtensionPlugin
{
public:
    /**
        Constructs the object and initializes default plugins according to
        application parameters.
        @param applicationParameters Application parameters
        @param extensionParameters Struture of pointers to objects that are
               potentially updated by registered plugins
    */
    DeflectPlugin( Brayns& brayns );

    /** @copydoc ExtensionPlugin::execute */
    BRAYNS_API void run( ) final;

private:
    struct HandledEvents
    {
        HandledEvents(
            const Vector2f& touchPosition,
            const Vector2f& wDelta,
            const bool pressedState,
            const bool exit )
            : position( touchPosition )
            , wheelDelta( wDelta )
            , pressed( pressedState )
            , closeApplication( exit )
        {
        }

        Vector2f position;     // Touch position provided by deflect
        Vector2f wheelDelta;   // Wheel delta provided by Deflect
        bool pressed;          // True if the touch is in pressed state
        bool closeApplication; // True if and EXIT event was received
    };

    void _initializeDeflect();
    void _sendDeflectFrame();
    void _handleDeflectEvents();

    /** Send an image to DisplayCluster
     *
     * @param imageSize size of the image
     * @param buffer containing the image
     * @param swapXAxis enables a horizontal flip operation on the image
     */
    void _send(
        const Vector2i& imageSize,
        unsigned long* imageData,
        bool swapXAxis);

    float _theta;
    float _phi;
    Vector3f _previousTouchPosition;
    std::unique_ptr< deflect::Stream > _stream;
    bool _pressed;
    bool _streamingEnabled;

};

}
#endif // DEFLECTPLUGIN_H
