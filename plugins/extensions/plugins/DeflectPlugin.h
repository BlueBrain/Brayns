/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
    DeflectPlugin(
        ApplicationParameters& applicationParameters,
        ExtensionParameters& extensionParameters );

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

    /** Handles touch events provided by DisplayCluster
     *
     * @param handledEvents Events populated by deflect
     * @return True if Deflect is available, false otherwise.
     */
    bool _handleTouchEvents( HandledEvents& handledEvents );

    float _theta;
    float _phi;
    Vector3f _previousTouchPosition;
    bool _compressImage;
    size_t _compressionQuality;
    std::unique_ptr<deflect::Stream> _stream;
};

}
#endif // DEFLECTPLUGIN_H
