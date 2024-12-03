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

#include "DeviceEndpoints.h"

#include "CameraEndpoints.h"
#include "FramebufferEndpoints.h"
#include "GeometryEndpoints.h"
#include "ImageEndpoints.h"
#include "ImageOperationEndpoints.h"
#include "LightEndpoints.h"
#include "MaterialEndpoints.h"
#include "RendererEndpoints.h"
#include "TextureEndpoints.h"
#include "TransferFunctionEndpoints.h"
#include "VolumeEndpoints.h"

namespace brayns
{
void addDeviceEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    addCameraEndpoints(builder, objects, device);
    addFramebufferEndpoints(builder, objects, device);
    addImageOperationEndpoints(builder, objects, device);
    addImageEndpoints(builder, objects);
    addTransferFunctionEndpoints(builder, objects, device);
    addVolumeEndpoints(builder, objects, device);
    addGeometryEndpoints(builder, objects, device);
    addTextureEndpoints(builder, objects, device);
    addMaterialEndpoints(builder, objects, device);
    addLightEndpoints(builder, objects, device);
    addRendererEndpoints(builder, objects, device);
}
}
