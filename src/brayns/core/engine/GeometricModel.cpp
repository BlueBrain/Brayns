/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "GeometricModel.h"

namespace
{
using namespace brayns;

void setGeometricModelParams(OSPGeometricModel handle, const Geometry &geometry, const GeometricModelSettings &settings)
{
    setObjectParam(handle, "geometry", geometry);
    setObjectParam(handle, "material", settings.materials);
    setObjectParam(handle, "color", settings.colors);
    setObjectParam(handle, "index", settings.indices);
    setObjectParam(handle, "invertNormals", settings.invertNormals);
    setObjectParam(handle, "id", settings.id);
}
}

namespace brayns
{
void GeometricModel::update(const Geometry &geometry, const GeometricModelSettings &settings)
{
    auto handle = getHandle();
    setGeometricModelParams(handle, geometry, settings);
    commitObject(handle);
}

GeometricModel createGeometricModel(Device &device, const Geometry &geometry, const GeometricModelSettings &settings)
{
    auto handle = ospNewGeometricModel();
    auto model = wrapObjectHandleAs<GeometricModel>(device, handle);

    setGeometricModelParams(handle, geometry, settings);

    commitObject(device, handle);

    return model;
}
}
