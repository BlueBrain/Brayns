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
const std::string CUDA_ATTR_CAMERA_BAD_COLOR = "bad_color";
const std::string CUDA_ATTR_CAMERA_OFFSET = "offset";
const std::string CUDA_ATTR_CAMERA_EYE = "eye";
const std::string CUDA_ATTR_CAMERA_U = "U";
const std::string CUDA_ATTR_CAMERA_V = "V";
const std::string CUDA_ATTR_CAMERA_W = "W";
const std::string CUDA_ATTR_CAMERA_APERTURE_RADIUS = "aperture_radius";
const std::string CUDA_ATTR_CAMERA_FOCAL_SCALE = "focal_scale";

const std::string CUDA_CLIP_PLANES = "clip_planes";
const std::string CUDA_NB_CLIP_PLANES = "nb_clip_planes";
} // namespace

namespace brayns
{
OptiXCamera::OptiXCamera()
{
    _camera = OptiXContext::get().createCamera();
}

OptiXCamera::~OptiXCamera()
{
    if (_camera)
        _camera->destroy();
}

void OptiXCamera::commit()
{
    auto context = OptiXContext::get().getOptixContext();

    Vector3d u, v, w;

    const Vector3d& pos = getPosition();

    _calculateCameraVariables(u, v, w);

    context[CUDA_ATTR_CAMERA_EYE]->setFloat(pos.x(), pos.y(), pos.z());
    context[CUDA_ATTR_CAMERA_U]->setFloat(u.x(), u.y(), u.z());
    context[CUDA_ATTR_CAMERA_V]->setFloat(v.x(), v.y(), v.z());
    context[CUDA_ATTR_CAMERA_W]->setFloat(w.x(), w.y(), w.z());
    context[CUDA_ATTR_CAMERA_APERTURE_RADIUS]->setFloat(
        getPropertyOrValue<double>("apertureRadius", 0.0));
    context[CUDA_ATTR_CAMERA_FOCAL_SCALE]->setFloat(
        getPropertyOrValue<double>("focusDistance", 1.0));
    context[CUDA_ATTR_CAMERA_BAD_COLOR]->setFloat(1.f, 0.f, 1.f);
    context[CUDA_ATTR_CAMERA_OFFSET]->setFloat(0, 0);

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

void OptiXCamera::_calculateCameraVariables(Vector3d& U, Vector3d& V,
                                            Vector3d& W)
{
    const auto& position = getPosition();
    const auto& up = getOrientation().rotate(Vector3f(0.0f, 1.0f, 0.0f));

    float ulen, vlen, wlen;
    W = getTarget() - position;

    wlen = W.length();
    U = normalize(cross(W, up));
    V = normalize(cross(U, W));

    vlen = wlen *
           tanf(0.5f * getPropertyOrValue<double>("fovy", 45.0) * M_PI / 180.f);
    V *= vlen;
    ulen = vlen * getPropertyOrValue<double>("aspect", 1.0);
    U *= ulen;
}

} // namespace brayns
