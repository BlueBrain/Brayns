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
#include <lexis/render/stream.h>

namespace brayns
{

#ifdef BRAYNS_USE_ZEROEQ
class ZeroEQPlugin;
#endif

struct Image
{
    std::vector<char> data;
    Vector2ui size;
};

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
#ifdef BRAYNS_USE_ZEROEQ
    DeflectPlugin( Brayns& brayns, ZeroEQPlugin& zeroeq );
#else
    DeflectPlugin( Brayns& brayns );
#endif

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
    bool _handleDeflectEvents();

    /** Send an image to DisplayCluster
     *
     * @param swapYAxis enables a vertical flip operation on the image
     */
    void _send( bool swapYAxis );

    Vector2d _getWindowPos( const deflect::Event& event ) const;
    double _getZoomDelta( const deflect::Event& pinchEvent ) const;

    Vector2d _previousPos;
    bool _pan = false;
    bool _pinch = false;
    std::unique_ptr< deflect::Stream > _stream;
    ::lexis::render::Stream _params;
    std::string _previousHost;
    Image _lastImage;
    deflect::Stream::Future _sendFuture;
};

}
#endif // DEFLECTPLUGIN_H
