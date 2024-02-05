/* Copyright 2018-2024 Blue Brain Project/EPFL
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

#include "../cylindricStereo/CylindricStereoCamera.h"

namespace ospray
{
/**
 * This camera is designed to work with the opendeck tracking system.
 * The rays are using cylindrical projection for the x axis and
 * perspective projection for the y axis of an image. This camera
 * can create a stereo pair of images.
 */
struct CylindricStereoTrackedCamera : public CylindricStereoCamera
{
    CylindricStereoTrackedCamera();
    std::string toString() const override;
    void commit() override;

private:
    vec3f _getHeadPosition();
    vec3f _getOpendeckCamDU();
};
} // namespace ospray
