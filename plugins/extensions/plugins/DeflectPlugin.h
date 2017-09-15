/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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
#if (BRAYNS_USE_NETWORKING)
class ZeroEQPlugin;
#endif

struct Image
{
    std::vector<char> data;
    Vector2ui size;
    FrameBufferFormat format;
};

class DeflectPlugin : public ExtensionPlugin
{
public:
#if (BRAYNS_USE_NETWORKING)
    DeflectPlugin(ZeroEQPlugin& zeroeq);
#else
    DeflectPlugin();
#endif

    /** @copydoc ExtensionPlugin::run */
    BRAYNS_API bool run(EnginePtr engine, KeyboardHandler& keyboardHandler,
                        AbstractManipulator& cameraManipulator) final;

private:
    struct HandledEvents
    {
        HandledEvents(const Vector2f& touchPosition, const Vector2f& wDelta,
                      const bool pressedState, const bool exit)
            : position(touchPosition)
            , wheelDelta(wDelta)
            , pressed(pressedState)
            , closeApplication(exit)
        {
        }

        Vector2f position;     // Touch position provided by deflect
        Vector2f wheelDelta;   // Wheel delta provided by Deflect
        bool pressed;          // True if the touch is in pressed state
        bool closeApplication; // True if and EXIT event was received
    };

    bool _initializeDeflect(bool observerOnly);
    void _sendDeflectFrame(Engine& engine);
    bool _handleDeflectEvents(Engine& engine, KeyboardHandler& keyboardHandler,
                              AbstractManipulator& cameraManipulator);

    /** Send an image to DisplayCluster
     *
     * @param swapYAxis enables a vertical flip operation on the image
     */
    void _send(const Engine& engine, bool swapYAxis);

    Vector2d _getWindowPos(const deflect::Event& event,
                           const Vector2ui& windowSize) const;
    double _getZoomDelta(const deflect::Event& pinchEvent,
                         const Vector2ui& windowSize) const;

    Vector2d _previousPos;
    bool _pan = false;
    bool _pinch = false;
    std::unique_ptr<deflect::Observer> _stream;
    ::lexis::render::Stream _params;
    std::string _previousHost;
    Image _lastImage;
    deflect::Stream::Future _sendFuture;
};
}
#endif // DEFLECTPLUGIN_H
