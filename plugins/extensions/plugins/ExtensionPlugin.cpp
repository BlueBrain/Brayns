/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "ExtensionPlugin.h"

namespace brayns
{

ExtensionPlugin::ExtensionPlugin(
    ApplicationParameters& applicationParameters,
    ExtensionParameters& extensionParameters )
    : _applicationParameters( applicationParameters )
    , _extensionParameters( extensionParameters )
{
}

}
