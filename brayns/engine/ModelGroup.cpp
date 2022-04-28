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

#include "ModelGroup.h"

#include <brayns/engine/common/DataHandler.h>

#include <algorithm>

namespace
{
struct HandleCommitter
{
    template<typename T>
    static void commit(const std::vector<T> &handleList, OSPDataType type, OSPGroup groupHandle, const char *id)
    {
        if (!handleList.empty())
        {
            auto buffer = brayns::DataHandler::copyBuffer(handleList, type);
            ospSetParam(groupHandle, id, OSPDataType::OSP_DATA, &buffer.handle);
        }
    }
};

struct ModelEraser
{
    template<typename T>
    static bool removeModel(std::vector<T> &modelList, T value)
    {
        auto it = std::find(modelList.begin(), modelList.end(), value);
        if (it != modelList.end())
        {
            modelList.erase(it);
            return true;
        }

        return false;
    }
};
}

namespace brayns
{
ModelGroup::ModelGroup()
    : _handle(ospNewGroup())
{
}

ModelGroup::~ModelGroup()
{
    ospRelease(_handle);
}

void ModelGroup::addGeometricModel(OSPGeometricModel model)
{
    _geometryModels.push_back(model);
    _modified = true;
}

void ModelGroup::removeGeometricModel(OSPGeometricModel model)
{
    _modified = _modified || ModelEraser::removeModel(_geometryModels, model);
}

void ModelGroup::addVolumetricModel(OSPVolumetricModel model)
{
    _volumeModels.push_back(model);
    _modified = true;
}

void ModelGroup::removeVolumetricModel(OSPVolumetricModel model)
{
    _modified = _modified || ModelEraser::removeModel(_volumeModels, model);
}

void ModelGroup::addClippingModel(OSPGeometricModel model)
{
    _clippingModels.push_back(model);
    _modified = true;
}

void ModelGroup::removeClippingModel(OSPGeometricModel model)
{
    _modified = _modified || ModelEraser::removeModel(_clippingModels, model);
}

OSPGroup ModelGroup::handle() const noexcept
{
    return _handle;
}

void ModelGroup::commit()
{
    if (!_modified)
    {
        return;
    }

    HandleCommitter::commit(_geometryModels, OSPDataType::OSP_GEOMETRIC_MODEL, _handle, "geometry");
    HandleCommitter::commit(_volumeModels, OSPDataType::OSP_VOLUMETRIC_MODEL, _handle, "volume");
    HandleCommitter::commit(_clippingModels, OSPDataType::OSP_GEOMETRIC_MODEL, _handle, "clippingGeometry");

    ospCommit(_handle);
}
}
