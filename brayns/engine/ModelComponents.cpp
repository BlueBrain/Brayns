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

#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>

namespace brayns
{
Model& Component::getModel()
{
    if(!_owner)
    {
        throw std::runtime_error("Component has not been initialized by Model");
    }

    return *_owner;
}

void Component::onStart()
{

}

void Component::onPreRender(const ParametersManager& params)
{
     (void)params;
}

void Component::onPostRender(const ParametersManager& params)
{
    (void)params;
}

void Component::onCommit()
{
}

void Component::onDestroyed()
{
}

Bounds Component::computeBounds(const Matrix4f& transform) const noexcept
{
    (void) transform;
    return {};
}

uint64_t ModelComponentContainer::getByteSize() const noexcept
{
    uint64_t result = 0;
    for(const auto& [index, component] : _components)
    {
        result += component->getSizeInBytes();
    }
    return result;
}

void ModelComponentContainer::onPreRender(const ParametersManager& params)
{
    for(auto& [index, component] : _components)
    {
        component->onPreRender(params);
    }
}

void ModelComponentContainer::onPostRender(const ParametersManager& params)
{
    for(auto& [index, component] : _components)
    {
        component->onPostRender(params);
    }
}

void ModelComponentContainer::onCommit()
{
    for(auto& [index, component] : _components)
    {
        component->onCommit();
    }
}

void ModelComponentContainer::onDestroyed()
{
    for(auto& [index, component] : _components)
    {
        component->onDestroyed();
    }
}
}
