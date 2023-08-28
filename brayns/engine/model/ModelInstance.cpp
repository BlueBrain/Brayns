/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "ModelInstance.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct InstanceParameters
{
    static inline const std::string transform = "transform";
};
}

namespace brayns
{
ModelInstance::ModelInstance(uint32_t instanceId, std::shared_ptr<Model> model):
    _id(instanceId),
    _model(std::move(model)),
    _handle(_model->getHandle())
{
    computeBounds();
}

ModelInstance::ModelInstance(uint32_t instanceId, const ModelInstance &other):
    _id(instanceId),
    _transform(other._transform),
    _bounds(other._bounds),
    _model(other._model),
    _handle(_model->getHandle())
{
}

uint32_t ModelInstance::getID() const noexcept
{
    return _id;
}

const Bounds &ModelInstance::getBounds() const noexcept
{
    return _bounds;
}

void ModelInstance::computeBounds() noexcept
{
    auto view = _model->getSystemsView();
    _bounds = view.computeBounds(_getFullTransform());
}

Model &ModelInstance::getModel() noexcept
{
    return *_model;
}

const Model &ModelInstance::getModel() const noexcept
{
    return *_model;
}

const std::string &ModelInstance::getModelType() const noexcept
{
    return _model->getType();
}

ModelInfo ModelInstance::getModelData() const noexcept
{
    return ModelInfo(*_model);
}

void ModelInstance::setVisible(const bool val) noexcept
{
    _flag.update(_visible, val);
}

bool ModelInstance::isVisible() const noexcept
{
    return _visible;
}

void ModelInstance::setTransform(const Transform &transform) noexcept
{
    if (_flag.update(_transform, transform))
    {
        auto matrix = _getFullTransform();
        auto view = _model->getSystemsView();
        _bounds = view.computeBounds(matrix);
    }
}

const Transform &ModelInstance::getTransform() const noexcept
{
    return _transform;
}

const ospray::cpp::Instance &ModelInstance::getHandle() const noexcept
{
    return _handle;
}

bool ModelInstance::commit()
{
    if (!_flag)
    {
        return false;
    }
    _updateTransform();
    _handle.commit();
    _flag.setModified(false);
    return true;
}

TransformMatrix ModelInstance::_getFullTransform() const noexcept
{
    auto matrix = _transform.toMatrix();

    auto &components = _model->getComponents();
    auto baseTransform = components.find<Transform>();
    if (baseTransform)
    {
        matrix = matrix * baseTransform->toMatrix();
    }

    return matrix;
}

void ModelInstance::_updateTransform()
{
    auto matrix = _getFullTransform();
    _handle.setParam(InstanceParameters::transform, matrix.affine);
}
}
