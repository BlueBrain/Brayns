/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include <ospray/SDK/common/Data.h>

#include "../Model.h"
#include "../render/Material.h"
#include "Instance.h"

#include <optix.h>
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

namespace bbp
{
namespace optix
{
std::string Instance::toString() const
{
    return "ospray::Instance";
}

Instance::~Instance()
{
    if (_transform)
        _transform->destroy();
}

void Instance::finalize(Model* parentModel)
{
    if (_transform)
        _transform->destroy();

    ospray::vec3f vx = getParam3f("xfm.l.vx", ospray::vec3f(0.f, 0.f, 0.f));
    ospray::vec3f vy = getParam3f("xfm.l.vy", ospray::vec3f(0.f, 0.f, 0.f));
    ospray::vec3f vz = getParam3f("xfm.l.vz", ospray::vec3f(0.f, 0.f, 0.f));
    ospray::vec3f p = getParam3f("xfm.p", ospray::vec3f(0.f, 0.f, 0.f));

    auto subModel = (Model*)getParamObject("model", nullptr);

    const float matrixData[16] = {vx.x, vx.y, vx.z, p.x, //
                                  vy.x, vy.y, vy.z, p.y, //
                                  vz.x, vz.y, vz.z, p.z, //
                                  0.0f, 0.0f, 0.0f, 1.0f};
    _transform = subModel->instance(::optix::Matrix4x4(matrixData));

    parentModel->addTransformInstance(_transform);
}
}
}
