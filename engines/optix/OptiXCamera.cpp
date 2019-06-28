/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "OptiXCamera.h"

#include <brayns/common/log.h>

namespace
{
const std::string CUDA_CLIP_PLANES = "clip_planes";
const std::string CUDA_NB_CLIP_PLANES = "nb_clip_planes";
} // namespace

namespace brayns
{
void OptiXCamera::commit()
{
    if (_currentCamera != getCurrentType())
    {
        _currentCamera = getCurrentType();
        OptiXContext::get().setCamera(_currentCamera);
    }

    auto cameraProgram = OptiXContext::get().getCamera(_currentCamera);

    auto context = OptiXContext::get().getOptixContext();

    cameraProgram->commit(*this, context);

    if (_clipPlanesBuffer)
        _clipPlanesBuffer->destroy();

    const size_t numClipPlanes = _clipPlanes.size();
    if (numClipPlanes > 0)
    {
        Vector4fs buffer;
        buffer.reserve(numClipPlanes);
        for (const auto& clipPlane : _clipPlanes)
            buffer.push_back({static_cast<float>(clipPlane[0]),
                              static_cast<float>(clipPlane[1]),
                              static_cast<float>(clipPlane[2]),
                              static_cast<float>(clipPlane[3])});

        _clipPlanesBuffer =
            context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4,
                                  numClipPlanes);
        memcpy(_clipPlanesBuffer->map(), buffer.data(),
               numClipPlanes * sizeof(Vector4f));
        _clipPlanesBuffer->unmap();
    }
    else
    {
        // Create empty buffer to avoid unset variable exception in cuda
        _clipPlanesBuffer =
            context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, 1);
    }

    context[CUDA_CLIP_PLANES]->setBuffer(_clipPlanesBuffer);
    context[CUDA_NB_CLIP_PLANES]->setUint(numClipPlanes);
}

} // namespace brayns
