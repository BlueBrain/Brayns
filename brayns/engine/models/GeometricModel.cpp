/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/models/GeometricModel.h>

#include <brayns/engine/materials/MatteMaterial.h>

namespace brayns
{
GeometricModel::GeometricModel()
 : _handle(ospNewGeometricModel())
 , _material(std::make_unique<MatteMaterial>())
{
}

GeometricModel::~GeometricModel()
{
    ospRelease(_handle);
}


void GeometricModel::setMaterial(Material::Ptr&& material)
{
    if(!material)
        throw std::invalid_argument("GeometricModel: Material cannot be null");

    _material = std::move(material);
    markModified(false);
}

const Material& GeometricModel::getMaterial() const noexcept
{
    return *_material;
}

void GeometricModel::commit()
{
    if(_material->isModified())
    {
        _material->doCommit();

        auto materialHandle = _material->handle();
        ospSetParam(_handle, "material", OSP_MATERIAL, &materialHandle);
    }

    commitGeometryModel();

    ospCommit(_handle);

    auto ospGroup = groupHandle();

    std::vector<OSPGeometricModel> modelList = {_handle};

    auto sharedModelList = ospNewSharedData(modelList.data(), OSPDataType::OSP_GEOMETRIC_MODEL, 1);
    auto copyModelList = ospNewData(OSPDataType::OSP_GEOMETRIC_MODEL, 1);
    ospCopyData(sharedModelList, copyModelList);

    ospSetParam(ospGroup, "geometry", OSPDataType::OSP_DATA, &copyModelList);

    ospRelease(sharedModelList);
    ospRelease(copyModelList);

    ospCommit(ospGroup);
}

OSPGeometricModel GeometricModel::handle() const noexcept
{
    return _handle;
}
}
