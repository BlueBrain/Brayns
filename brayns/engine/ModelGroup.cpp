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

#include <brayns/engine/ModelGroup.h>

#include <algorithm>

namespace
{
template<typename T>
void copyListToOSPRay(OSPGroup handle, std::vector<T> &ospModels, const OSPDataType type, const char* ID) noexcept
{
    if(ospModels.empty())
    {
        return;
    }

    auto sharedList = ospNewSharedData(ospModels.data(), type, ospModels.size());
    auto copiedList = ospNewData(type, 1);
    ospCopyData(sharedList, copiedList);
    ospRelease(sharedList);
    ospSetParam(handle, ID, OSPDataType::OSP_DATA, &copiedList);
    ospRelease(copiedList);
}

template<typename T>
bool removeModel(std::vector<T> &modelList, T value)
{
    auto it = std::find(modelList.begin(), modelList.end(), value);
    if(it != modelList.end())
    {
        modelList.erase(it);
        return true;
    }

    return false;
}
}

namespace brayns
{
ModelGroup::ModelGroup()
{
    _handle = ospNewGroup();
}

ModelGroup::~ModelGroup()
{
    if(_handle)
        ospRelease(_handle);
}

void ModelGroup::addGeometricModel(OSPGeometricModel model)
{
    _geometryModels.push_back(model);
    _modified = true;
}

void ModelGroup::removeGeometricModel(OSPGeometricModel model)
{
    _modified = _modified || removeModel(_geometryModels, model);
}

void ModelGroup::addVolumetricModel(OSPVolumetricModel model)
{
    _volumeModels.push_back(model);
    _modified = true;
}

void ModelGroup::removeVolumetricModel(OSPVolumetricModel model)
{
    _modified = _modified || removeModel(_volumeModels, model);
}

void ModelGroup::addClippingModel(OSPGeometricModel model)
{
    _clippingModels.push_back(model);
    _modified = true;
}

void ModelGroup::removeClippingModel(OSPGeometricModel model)
{
    _modified = _modified || removeModel(_clippingModels, model);
}

OSPGroup ModelGroup::handle() const noexcept
{
    return _handle;
}

void ModelGroup::commit()
{
    if(!_modified)
    {
        return;
    }

    copyListToOSPRay(_handle, _geometryModels, OSPDataType::OSP_GEOMETRIC_MODEL, "geometry");
    copyListToOSPRay(_handle, _volumeModels, OSPDataType::OSP_VOLUMETRIC_MODEL, "volume");
    copyListToOSPRay(_handle, _clippingModels, OSPDataType::OSP_GEOMETRIC_MODEL, "clippingGeometry");

    ospCommit(_handle);
}
}
