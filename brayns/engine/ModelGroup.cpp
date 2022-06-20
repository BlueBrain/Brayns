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

#include <ospray/ospray_cpp/Data.h>

#include <algorithm>

namespace
{
struct GroupParameters
{
    inline static const std::string geometry = "geometry";
    inline static const std::string volume = "volume";
    inline static const std::string clipping = "clippingGeometry";
};

class GroupParameterUpdater
{
public:
    GroupParameterUpdater(const ospray::cpp::Group &group)
        : _group(group)
    {
    }

    template<typename T>
    void update(const std::string &name, const std::vector<T> &handleList)
    {
        if (handleList.empty())
        {
            _group.removeParam(name);
            return;
        }
        _group.setParam(name, ospray::cpp::CopiedData(handleList));
    }

private:
    const ospray::cpp::Group &_group;
};

class ModelEraser
{
public:
    template<typename StoredData, typename SourceData>
    static bool removeModel(std::vector<StoredData> &modelList, const SourceData &value)
    {
        const auto &object = value.getOsprayObject();
        auto handle = object.handle();
        auto it = std::find_if(
            modelList.begin(),
            modelList.end(),
            [&](auto &osprayObject) { return osprayObject.handle() == handle; });
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
void ModelGroup::addGeometricModel(const GeometryObject &model)
{
    _geometryModels.push_back(model.getOsprayObject());
    _modified = true;
}

void ModelGroup::removeGeometricModel(const GeometryObject &model)
{
    _modified = ModelEraser::removeModel(_geometryModels, model) || _modified;
}

void ModelGroup::addVolumetricModel(const VolumeObject &model)
{
    _volumeModels.push_back(model.getOsprayObject());
    _modified = true;
}

void ModelGroup::removeVolumetricModel(const VolumeObject &model)
{
    _modified = ModelEraser::removeModel(_volumeModels, model) || _modified;
}

void ModelGroup::addClippingModel(const GeometryObject &model)
{
    _clippingModels.push_back(model.getOsprayObject());
    _modified = true;
}

void ModelGroup::removeClippingModel(const GeometryObject &model)
{
    _modified = ModelEraser::removeModel(_clippingModels, model) || _modified;
}

ospray::cpp::Group &ModelGroup::getOsprayGroup() noexcept
{
    return _osprayGroup;
}

bool ModelGroup::commit()
{
    if (!_modified)
    {
        return false;
    }
    GroupParameterUpdater updater(_osprayGroup);
    updater.update(GroupParameters::geometry, _geometryModels);
    updater.update(GroupParameters::volume, _volumeModels);
    updater.update(GroupParameters::clipping, _clippingModels);
    _osprayGroup.commit();
    return true;
}
}
