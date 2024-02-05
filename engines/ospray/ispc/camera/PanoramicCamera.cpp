/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
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

#include "PanoramicCamera.h"
#include <limits>
// ispc-side stuff
#include "PanoramicCamera_ispc.h"

#include <ospray/SDK/common/Data.h>

#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <math.h> // M_PI
#endif

namespace ospray
{
PanoramicCamera::PanoramicCamera()
{
    ispcEquivalent = ispc::PanoramicCamera_create(this);
}

void PanoramicCamera::commit()
{
    Camera::commit();

    // ------------------------------------------------------------------
    // first, "parse" the additional expected parameters
    // ------------------------------------------------------------------
    // FIXME(jonask): When supported by OSPRay use bool
    stereo = getParam("stereo", 0);
    half = getParam("half", 0);
    // the default 63.5mm represents the average human IPD
    interpupillaryDistance = getParamf("interpupillaryDistance", 0.0635f);
    enableClippingPlanes = getParam("enableClippingPlanes", 0);
    clipPlanes =
        enableClippingPlanes ? getParamData("clipPlanes", nullptr) : nullptr;

    // ------------------------------------------------------------------
    // now, update the local precomputed values
    // ------------------------------------------------------------------
    dir = normalize(dir);
    vec3f dirU = normalize(cross(dir, up));
    vec3f dirV = cross(dirU, dir); // rotate film to be perpendicular to 'dir'

    vec3f org = pos;
    const vec3f ipd_offset = 0.5f * interpupillaryDistance * dirU;

    if (stereo)
    {
        auto bufferTarget = getParamString("buffer_target");
        if (bufferTarget.length() == 2)
        {
            if (bufferTarget.at(1) == 'L')
                org -= ipd_offset;
            if (bufferTarget.at(1) == 'R')
                org += ipd_offset;
        }
    }

    const auto clipPlaneData = clipPlanes ? clipPlanes->data : nullptr;
    const size_t numClipPlanes = clipPlanes ? clipPlanes->numItems : 0;

    ispc::PanoramicCamera_set(getIE(), (const ispc::vec3f&)org,
                              (const ispc::vec3f&)dir, (const ispc::vec3f&)dirU,
                              (const ispc::vec3f&)dirV,
                              (const ispc::vec3f&)ipd_offset,
                              (const ispc::vec4f*)clipPlaneData, numClipPlanes,
                              half);
}

OSP_REGISTER_CAMERA(PanoramicCamera, panoramic);

} // namespace ospray
