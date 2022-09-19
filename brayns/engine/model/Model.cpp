/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/engine/components/Clippers.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/Lights.h>
#include <brayns/engine/components/Volumes.h>

namespace
{
class GroupBuilder
{
public:
    static ospray::cpp::Group build(brayns::Components &components)
    {
        ospray::cpp::Group group;
        _addGeometry(components, group);
        _addVolume(components, group);
        _addClippers(components, group);
        _addLights(components, group);
        group.commit();
        return group;
    }

private:
    template<typename HandleType, typename ComponentType>
    static std::vector<HandleType> _compileHandles(brayns::Components &components)
    {
        std::vector<HandleType> result;
        if (auto component = components.find<ComponentType>())
        {
            result.reserve(component.elements.size());
            for (auto &element : component.elements)
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

    static void _addGeometry(brayns::Components &components, ospray::cpp::Group &group)
    {
        inline static const std::string param = "geometry";
        _add<ospray::cpp::GeometricModel, brayns::GeometryViews>(components, group, param);
    }

    static void _addVolume(brayns::Components &components, ospray::cpp::Group &group)
    {
        inline static const std::string param = "volume";
    }

    static void _addClippers(brayns::Components &components, ospray::cpp::Group &group)
    {
        inline static const std::string param = "clippingGeometry";
    }

    static void _addLights(brayns::Components &components, ospray::cpp::Group &group)
    {
        inline static const std::string param = "light";
    }
};
}

namespace brayns
{
uint32_t Model::getID() const noexcept
{
    return _modelId;
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

InspectResultData Model::inspect(const InspectContext &context)
{
    return _systems.inspect(context, _components);
}

Bounds Model::computeBounds(const Matrix4f &matrix)
{
    return _systems.computeBounds(matrix, _components);
}

void Model::init()
{
    _systems.init(_components);
    _systems.commit(_components);
    _handle = GroupBuilder::build(_components);
}

void Model::onPreRender(const ParametersManager &parameters)
{
    _systems.preRender(parameters, _components);
}

CommitResult Model::commit()
{
    return _systems.commit(_components);
}

void Model::onPostRender(const ParametersManager &parameters)
{
    return _systems.postRender(parameters, _components);
}
} // namespace brayns
