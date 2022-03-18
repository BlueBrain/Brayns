/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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


#include "ModelInstanceAdapter.h"

namespace brayns
{
ModelInstanceProxy::ModelInstanceProxy(ModelInstance &modelInstance)
 : _modelInstance(&modelInstance)
{
}

uint32_t ModelInstanceProxy::getID() const noexcept
{
    return _modelInstance->getID();
}

const Bounds &ModelInstanceProxy::getBounds() const noexcept
{
    return _modelInstance->getBounds();
}

const std::map<std::string, std::string> &ModelInstanceProxy::getModelMetadata() const noexcept
{
    return _modelInstance->getModelMetadata();
}

const Transformation &ModelInstanceProxy::getTransform() const noexcept
{
    return _modelInstance->getTransform();
}

void ModelInstanceProxy::setTransform(const Transformation &transform) noexcept
{
    _modelInstance->setTransform(transform);
}

bool ModelInstanceProxy::isVisible() const noexcept
{
    return _modelInstance->isVisible();
}

void ModelInstanceProxy::setVisible(const bool val) noexcept
{
    _modelInstance->setVisible(val);
}
}
