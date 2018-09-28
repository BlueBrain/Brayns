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

#pragma once

#include "camera/Camera.h"

namespace ospray
{
//! Implements a clipped perspective camera
struct OSPRAY_SDK_INTERFACE PerspectiveCamera : public Camera
{
    /*! \brief constructor \internal also creates the ispc-side data structure
     */
    PerspectiveCamera();

    //! \brief common function to help printf-debugging
    /*! Every derived class should override this! */
    virtual std::string toString() const
    {
        return "ospray::PerspectiveCamera";
    }
    virtual void commit();

public:
    // ------------------------------------------------------------------
    // the parameters we 'parsed' from our parameters
    // ------------------------------------------------------------------
    float fovy;
    float aspect;
    float apertureRadius;
    float focusDistance;
    bool architectural; // orient image plane to be parallel to 'up' and shift
                        // the lens
    typedef enum {
        OSP_STEREO_NONE,
        OSP_STEREO_LEFT,
        OSP_STEREO_RIGHT,
        OSP_STEREO_SIDE_BY_SIDE
    } StereoMode;
    StereoMode stereoMode;
    float interpupillaryDistance; // distance between the two cameras (stereo)

    // Clip planes
    Ref<Data> clipPlanes;
};

} // ::ospray
