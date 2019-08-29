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
//! Implements a clipped panoramic camera
struct OSPRAY_SDK_INTERFACE PanoramicCamera : public Camera
{
    /*! \brief constructor \internal also creates the ispc-side data structure
     */
    PanoramicCamera();

    //! \brief common function to help printf-debugging
    /*! Every derived class should override this! */
    virtual std::string toString() const { return "ospray::PanoramicCamera"; }
    virtual void commit();

public:
    // ------------------------------------------------------------------
    // the parameters we 'parsed' from our parameters
    // ------------------------------------------------------------------
    bool stereo;
    float interpupillaryDistance; // distance between the two cameras (stereo)

    // Clip planes
    bool enableClippingPlanes{false};
    Ref<Data> clipPlanes;

    // Dome
    bool half{false};
};

} // namespace ospray
