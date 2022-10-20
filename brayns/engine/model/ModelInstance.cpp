/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <ospray/SDK/common/OSPCommon.h>

namespace
{
class MatrixConverter
{
public:
    static auto glmToOspray(const brayns::Matrix4f &transform) noexcept
    {
        const auto &strafe = transform[0];
        const auto &up = transform[1];
        const auto &forward = transform[2];
        const auto &position = transform[3];

        rkcommon::math::affine3f result;
        result.l.vx = rkcommon::math::vec3f(strafe.x, strafe.y, strafe.z);
        result.l.vy = rkcommon::math::vec3f(up.x, up.y, up.z);
        result.l.vz = rkcommon::math::vec3f(forward.x, forward.y, forward.z);
        result.p = rkcommon::math::vec3f(position.x, position.y, position.z);

        return result;
    }
};

struct InstanceParameters
{
    inline static const std::string transform = "transform";
};
}

namespace brayns
{
ModelInstance::ModelInstance(uint32_t instanceId, std::shared_ptr<Model> model)
    : _id(instanceId)
    , _model(std::move(model))
    , _handle(_model->getHandle())
{
    computeBounds();
}

ModelInstance::ModelInstance(uint32_t instanceId, const ModelInstance &other)
    : ModelInstance(instanceId, other._model)
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
    _bounds = _model->computeBounds(_getFullTransform());
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
        auto affine = MatrixConverter::glmToOspray(matrix);
        _handle.setParam(InstanceParameters::transform, affine);
        _bounds = _model->computeBounds(matrix);
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
    _handle.commit();
    _flag.setModified(false);
    return true;
}

Matrix4f ModelInstance::_getFullTransform() const noexcept
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
}
