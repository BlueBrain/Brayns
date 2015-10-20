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

#ifndef EXTENSIONCONTROLLER_H
#define EXTENSIONCONTROLLER_H

#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/ui/BaseWindow.h>

namespace brayns
{

class ExtensionControllerImpl;

struct ExtensionParameters
{
    OSPRenderer&    renderer;
    OSPCamera&      camera;
    uint32*         imageBuffer;
    OSPFrameBuffer& frameBuffer;
    vec2i           windowSize;
    box3f           bounds;
    bool            running;
};

/** Manages extensions such as the RESTBridge of the Deflect Streamer
 */
class ExtensionController
{
public:
    /** Creates and initializes extensions
     *  according to their availability
     *
     * @param argc Number of arguments
     * @param argv Arguments to pass to the extensions
     */
    ExtensionController(
        const ApplicationParameters& applicationParameters,
        ExtensionParameters& extensionParameters );

    ~ExtensionController();

    /** Executes code specific to every registered extension
     */
    void execute();

    /** Returns the current state of the extension parameters
     */
    ExtensionParameters& getParameters();

private:
    std::unique_ptr<ExtensionControllerImpl> _impl;
};

}

#endif // EXTENSIONCONTROLLER_H
