/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "Camera.h"

namespace
{
using namespace brayns::experimental;

void setCameraParams(OSPCamera handle, const CameraSettings &settings)
{
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "nearClip", settings.nearClippingDistance);
}
}

namespace brayns::experimental
{
void ObjectReflector<PerspectiveCamera>::loadParams(OSPCamera handle, const Settings &settings)
{
    setCameraParams(handle, settings);
    setObjectParam(handle, "fovy", settings.fovy);
    setObjectParam(handle, "aspect", settings.aspectRatio);
    commitObject(handle);
}

void ObjectReflector<OrthographicCamera>::loadParams(OSPCamera handle, const Settings &settings)
{
    setCameraParams(handle, settings);
    setObjectParam(handle, "height", settings.height);
    setObjectParam(handle, "aspect", settings.aspectRatio);
    commitObject(handle);
}
}
