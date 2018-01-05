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

#pragma once

#include <ospray/SDK/camera/Camera.h>

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

namespace bbp
{
namespace optix
{
struct Camera : public ospray::Camera
{
    Camera();
    ~Camera();

    std::string toString() const override;
    void commit() override;

private:
    ::optix::Context _context;
    ::optix::Program _camera{nullptr};
    std::array<::optix::Program, 2> _missProgram;
};
}
}
