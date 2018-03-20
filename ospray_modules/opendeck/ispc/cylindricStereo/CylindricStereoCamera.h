/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#include <ospray/SDK/camera/Camera.h>

namespace ospray
{
/**
 * This camera is designed for the opendeck. It has a fixed
 * vertical field of view of 48.549 degrees. The rays are using
 * cylindrical projection for the x axis and perspective projection
 * for the y axis of an image. This camera create an omnidirectional
 * stereo pair of images.
 */
struct CylindricStereoCamera : public Camera
{
    CylindricStereoCamera();
    virtual ~CylindricStereoCamera() override = default;
    virtual std::string toString() const override;
    virtual void commit() override;

    typedef enum {
        OSP_STEREO_NONE,
        OSP_STEREO_LEFT,
        OSP_STEREO_RIGHT,
        OSP_STEREO_SIDE_BY_SIDE
    } StereoMode;
};

} // ::ospray
