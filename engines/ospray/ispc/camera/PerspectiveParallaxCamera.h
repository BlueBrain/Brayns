/* Copyright 2018-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
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

#include <ospray/SDK/camera/Camera.h>

namespace ospray
{
/**
 * This camera is an extension of the regular ospray
 * stereoscopic camera. It has an additional option
 * to select the distance of the zero-parallax plane.
 */
struct PerspectiveParallaxCamera : public Camera
{
    PerspectiveParallaxCamera();
    virtual ~PerspectiveParallaxCamera() override = default;

    //! \brief common function to help printf-debugging
    /*! Every derived class should override this! */
    virtual std::string toString() const override
    {
        return "ospray::PerspectiveParallaxCamera";
    }
    virtual void commit() override;

    typedef enum
    {
        OSP_STEREO_NONE,
        OSP_STEREO_LEFT,
        OSP_STEREO_RIGHT,
        OSP_STEREO_SIDE_BY_SIDE
    } StereoMode;
};

} // namespace ospray
