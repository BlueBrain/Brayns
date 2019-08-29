// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

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
    stereo = getParam("stereo", 0u);
    half = getParam("half", 0u);
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
    vec3f dirV =
        cross(dirU, dir); // rotate film to be perpendicular to 'dir'

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
                              (const ispc::vec3f&)dir, 
                              (const ispc::vec3f&)dirU,
                              (const ispc::vec3f&)dirV,
                              (const ispc::vec3f&)ipd_offset,
                              (const ispc::vec4f*)clipPlaneData,
                              numClipPlanes,
                              (const bool)half);
}

OSP_REGISTER_CAMERA(PanoramicCamera, panoramic);

} // namespace ospray
