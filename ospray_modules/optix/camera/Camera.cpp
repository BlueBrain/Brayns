/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include "../Context.h"

#include <ospray_module_optix_generated_Camera.cu.ptx.h>
#include <ospray_module_optix_generated_Constantbg.cu.ptx.h>

#include <ospray/SDK/common/Data.h>

namespace bbp
{
namespace optix
{
char const* const CUDA_PERSPECTIVE_CAMERA =
    ospray_module_optix_generated_Camera_cu_ptx;
char const* const CUDA_MISS = ospray_module_optix_generated_Constantbg_cu_ptx;
const std::string CUDA_FUNCTION_CAMERA = "camera";
const std::string CUDA_FUNCTION_EXCEPTION = "exception";
const std::string CUDA_FUNCTION_ENVMAP_MISS = "envmap_miss";
const std::string CUDA_FUNCTION_MISS = "miss";

const std::string CUDA_ATTRIBUTE_BAD_COLOR = "bad_color";
const std::string CUDA_ATTRIBUTE_CAMERA_OFFSET = "offset";
const std::string CUDA_ATTRIBUTE_CAMERA_EYE = "eye";
const std::string CUDA_ATTRIBUTE_CAMERA_U = "U";
const std::string CUDA_ATTRIBUTE_CAMERA_V = "V";
const std::string CUDA_ATTRIBUTE_CAMERA_W = "W";
const std::string CUDA_ATTRIBUTE_CAMERA_APERTURE_RADIUS = "aperture_radius";
const std::string CUDA_ATTRIBUTE_CAMERA_FOCAL_SCALE = "focal_scale";

const std::string CUDA_CLIP_PLANES[6] = {"clip_plane1", "clip_plane2",
                                         "clip_plane3", "clip_plane4",
                                         "clip_plane5", "clip_plane6"};
const std::string CUDA_NUM_CLIP_PLANES = "num_clip_planes";

Camera::Camera()
{
    _context = Context::get().getOptixContext();

    std::string cameraName = CUDA_FUNCTION_CAMERA;
    std::string cameraPtx = CUDA_PERSPECTIVE_CAMERA;

    // Exception program
    _context->setExceptionProgram(0, _context->createProgramFromPTXString(
                                         cameraPtx, CUDA_FUNCTION_EXCEPTION));

    // Ray generation program
    _camera = _context->createProgramFromPTXString(cameraPtx, cameraName);

    // Miss programs
    _missProgram[0] =
        _context->createProgramFromPTXString(CUDA_MISS, CUDA_FUNCTION_MISS);
    _missProgram[1] =
        _context->createProgramFromPTXString(CUDA_MISS,
                                             CUDA_FUNCTION_ENVMAP_MISS);

    _context[CUDA_ATTRIBUTE_BAD_COLOR]->setFloat(1.f, 0.f, 1.f);
    _context[CUDA_ATTRIBUTE_CAMERA_OFFSET]->setFloat(0, 0);

    _context->setRayGenerationProgram(0, _camera);
}

Camera::~Camera()
{
    if (_camera)
        _camera->destroy();

    for (auto& i : _missProgram)
        if (i)
            i->destroy();
}

std::string Camera::toString() const
{
    return "ospray::Camera";
}

void Camera::commit()
{
    // base class params
    pos = getParam3f("pos", ospray::vec3f(0.f));
    dir = getParam3f("dir", ospray::vec3f(0.f, 0.f, 1.f));
    up = getParam3f("up", ospray::vec3f(0.f, 1.f, 0.f));
    nearClip = getParam1f("nearClip", getParam1f("near_clip", 1e-6f));
    imageStart =
        getParam2f("imageStart", getParam2f("image_start", ospray::vec2f(0.f)));
    imageEnd =
        getParam2f("imageEnd", getParam2f("image_end", ospray::vec2f(1.f)));
    shutterOpen = getParam1f("shutterOpen", 0.0f);
    shutterClose = getParam1f("shutterClose", 0.0f);

    // "our" params
    const float fovy = getParamf("fovy", 60.f);
    const float aspect = getParamf("aspect", 1.f);
    const float apertureRadius = getParamf("apertureRadius", 0.f);
    const float focusDistance = getParamf("focusDistance", 1.f);

    // Miss program
    const bool environmentMap = getParam1i("environmentMap", 0) == 1;
    _context->setMissProgram(0, _missProgram[environmentMap]);

    ospray::vec3f u, v, w;
    {
        float ulen, vlen, wlen;
        w = dir;

        wlen = length(w);
        u = normalize(cross(w, up));
        v = normalize(cross(u, w));

        vlen = wlen * tanf(0.5f * fovy * M_PI / 180.f);
        v *= vlen;
        ulen = vlen * aspect;
        u *= ulen;
    }

    _context[CUDA_ATTRIBUTE_CAMERA_EYE]->setFloat(pos.x, pos.y, pos.z);
    _context[CUDA_ATTRIBUTE_CAMERA_U]->setFloat(u.x, u.y, u.z);
    _context[CUDA_ATTRIBUTE_CAMERA_V]->setFloat(v.x, v.y, v.z);
    _context[CUDA_ATTRIBUTE_CAMERA_W]->setFloat(w.x, w.y, w.z);
    _context[CUDA_ATTRIBUTE_CAMERA_APERTURE_RADIUS]->setFloat(apertureRadius);
    _context[CUDA_ATTRIBUTE_CAMERA_FOCAL_SCALE]->setFloat(focusDistance);

    ospray::Ref<ospray::Data> clipPlanes = getParamData("clipPlanes", nullptr);
    const auto clipPlaneData =
        static_cast<ospray::vec4f*>(clipPlanes ? clipPlanes->data : nullptr);
    const size_t numClipPlanes = clipPlanes ? clipPlanes->numItems : 0;

    for (size_t i = 0; i < 6 && i < numClipPlanes; ++i)
    {
        const auto& clipPlane = clipPlaneData[i];
        _context[CUDA_CLIP_PLANES[i]]->setFloat(clipPlane.x, clipPlane.y,
                                                clipPlane.z, clipPlane.w);
    }
    _context[CUDA_NUM_CLIP_PLANES]->setUint(std::min(6ul, numClipPlanes));
}

OSP_REGISTER_CAMERA(Camera, perspective);
}
}
