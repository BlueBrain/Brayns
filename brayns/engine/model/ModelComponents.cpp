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

#include "ModelComponents.h"
#include "Model.h"

namespace brayns
{
Model &Component::getModel()
{
    assert(_owner);
    return *_owner;
}

void Component::onCreate()
{
}

void Component::onPreRender(const ParametersManager &params)
{
    (void)params;
}

void Component::onPostRender(const ParametersManager &params)
{
    (void)params;
}

bool Component::commit()
{
    return false;
}

void Component::onDestroy()
{
}

Bounds Component::computeBounds(const Matrix4f &transform) const noexcept
{
    (void)transform;
    return {};
}

void Component::onInspect(const InspectContext &context, JsonObject &writeResult) const noexcept
{
    (void)context;
    (void)writeResult;
}

void ModelComponentContainer::onPreRender(const ParametersManager &params)
{
    for (auto &entry : _components)
    {
        auto &component = *entry.component;
        component.onPreRender(params);
    }
}

void ModelComponentContainer::onPostRender(const ParametersManager &params)
{
    for (auto &entry : _components)
    {
        auto &component = *entry.component;
        component.onPostRender(params);
    }
}

bool ModelComponentContainer::commit()
{
    bool committed = false;
    for (auto &entry : _components)
    {
        auto &component = *entry.component;
        committed = component.commit() || committed;
    }
    return committed;
}

void ModelComponentContainer::onDestroy()
{
    for (auto &entry : _components)
    {
        auto &component = *entry.component;
        component.onDestroy();
    }
}

Bounds ModelComponentContainer::computeBounds(const Matrix4f &transform) const noexcept
{
    Bounds result;
    for (auto &entry : _components)
    {
        auto &component = *entry.component;
        auto componentBounds = component.computeBounds(transform);
        result.expand(componentBounds);
    }
    return result;
}

void ModelComponentContainer::onInspect(const InspectContext &context, JsonObject &writeResult) const noexcept
{
    for (const auto &entry : _components)
    {
        const auto &component = *entry.component;
        component.onInspect(context, writeResult);
    }
}
}
