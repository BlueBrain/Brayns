/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/network/adapters/ModelInstanceAdapter.h>

namespace brayns
{
ReadModelProxy::ReadModelProxy(const ModelInstance &mi)
    : _instance(&mi)
{
}

uint32_t ReadModelProxy::getId() const
{
    const auto &mi = getModel();
    return mi.getID();
}

const Bounds &ReadModelProxy::getBounds() const
{
    const auto &mi = getModel();
    return mi.getBounds();
}

const Model::Metadata &ReadModelProxy::getMetadata() const
{
    const auto &mi = getModel();
    return mi.getModelMetadata();
}

const Transformation &ReadModelProxy::getTransform() const
{
    const auto &mi = getModel();
    return mi.getTransform();
}

bool ReadModelProxy::getIsVisible() const
{
    const auto &mi = getModel();
    return mi.isVisible();
}

const ModelInstance &ReadModelProxy::getModel() const
{
    if (!_instance)
    {
        throw std::runtime_error("ModelInstance not set in ReadModelProxy");
    }
    return *_instance;
}

UpdateModelProxy::UpdateModelProxy(Scene &scene)
    : _scene(scene)
{
}

void UpdateModelProxy::setId(uint32_t id)
{
    _modelInstance = &_scene.getModelInstance(id);
}

void UpdateModelProxy::setTransformation(const Transformation &transformation)
{
    _modelInstance->setTransform(transformation);
}

void UpdateModelProxy::setVisible(bool visible)
{
    _modelInstance->setVisible(visible);
}
}
