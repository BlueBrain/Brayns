/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "Model.h"

#include <brayns/core/engine/components/ClipperViews.h>
#include <brayns/core/engine/components/GeometryViews.h>
#include <brayns/core/engine/components/Lights.h>
#include <brayns/core/engine/components/VolumeViews.h>

namespace
{
class GroupBuilder
{
public:
    static ospray::cpp::Group build(brayns::Components &components)
    {
        ospray::cpp::Group group;
        _add<ospray::cpp::GeometricModel, brayns::GeometryViews>(components, group, _geometryParam);
        _add<ospray::cpp::VolumetricModel, brayns::VolumeViews>(components, group, _volumeParam);
        _add<ospray::cpp::GeometricModel, brayns::ClipperViews>(components, group, _clippingParam);
        _add<ospray::cpp::Light, brayns::Lights>(components, group, _lightParam);
        group.commit();
        return group;
    }

private:
    static inline const std::string _geometryParam = "geometry";
    static inline const std::string _volumeParam = "volume";
    static inline const std::string _clippingParam = "clippingGeometry";
    static inline const std::string _lightParam = "light";

    template<typename HandleType, typename ComponentType>
    static std::vector<HandleType> _compileHandles(brayns::Components &components)
    {
        std::vector<HandleType> result;
        if (auto component = components.find<ComponentType>())
        {
            result.reserve(component->elements.size());
            for (auto &element : component->elements)
            {
                result.push_back(element.getHandle());
            }
        }
        return result;
    }

    template<typename HandleType, typename ComponentType>
    static void _add(brayns::Components &components, ospray::cpp::Group &group, const std::string &param)
    {
        auto handles = _compileHandles<HandleType, ComponentType>(components);
        if (!handles.empty())
        {
            group.setParam(param, ospray::cpp::CopiedData(handles));
        }
    }
};
}

namespace brayns
{
Model::Model(std::string type):
    _type(std::move(type))
{
}

uint32_t Model::getID() const noexcept
{
    return _id;
}

const std::string &Model::getType() const noexcept
{
    return _type;
}

ospray::cpp::Group &Model::getHandle() noexcept
{
    return _handle;
}

Components &Model::getComponents() noexcept
{
    return _components;
}

const Components &Model::getComponents() const noexcept
{
    return _components;
}

Systems &Model::getSystems() noexcept
{
    return _systems;
}

SystemsView Model::getSystemsView() noexcept
{
    return SystemsView(_systems, _components);
}

void Model::init()
{
    if (_systems._data)
    {
        _systems._data->init(_components);
        _systems._data->commit(_components);
    }

    _handle = GroupBuilder::build(_components);
}

CommitResult Model::commit()
{
    if (!_systems._data)
    {
        return {};
    }

    auto result = _systems._data->commit(_components);
    if (result.needsRebuildBVH)
    {
        _handle.commit();
    }
    return result;
}
} // namespace brayns
