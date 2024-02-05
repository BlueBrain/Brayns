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

#pragma once

#include "camera/Camera.h"

namespace ospray
{
//! Implements a clipped FishEye camera
struct OSPRAY_SDK_INTERFACE FishEyeCamera : public Camera
{
    /*! \brief constructor \internal also creates the ispc-side data structure
     */
    FishEyeCamera();

    //! \brief common function to help printf-debugging
    /*! Every derived class should override this! */
    virtual std::string toString() const { return "ospray::FishEyeCamera"; }
    virtual void commit();

public:
    // Clip planes
    bool enableClippingPlanes{false};
    Ref<Data> clipPlanes;

    float apertureRadius;
    float focusDistance;
};

} // namespace ospray
