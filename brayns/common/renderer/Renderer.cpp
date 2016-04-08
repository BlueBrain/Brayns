/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/common/types.h>

#include "Renderer.h"

namespace brayns
{

Renderer::Renderer( RenderingParameters& renderingParameters )
    : _renderingParameters( renderingParameters )
{
}

}
