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
#include "RenderGroup.h"

#include <brayns/common/Log.h>

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

class ExtractGroupHandle
{
public:
    static OSPGroup extract(brayns::Model &model)
    {
        auto &components = model.getComponents();
        auto &group = components.get<brayns::RenderGroup>();
        return group.getHandle().handle();
    }
};

struct InstanceParameters
{
    inline static const std::string transform = "transform";
};
}

namespace brayns
{
ModelInstance::ModelInstance(const uint32_t modelInstanceID, Model &model)
    : _modelInstanceID(modelInstanceID)
    , _model(model)
    , _osprayInstance(ExtractGroupHandle::extract(model))
{
    computeBounds();
}

uint32_t ModelInstance::getID() const noexcept
{
    return _modelInstanceID;
}

const Bounds &ModelInstance::getBounds() const noexcept
{
    return _bounds;
}

void ModelInstance::computeBounds() noexcept
{
    Log::debug("[ModelInstance {}] Computing bounds", _modelInstanceID);

    const auto matrix = _transform.toMatrix();
    _bounds = _model.computeBounds(matrix);

    // In the event that the model used by this instance has no geometry,
    // The result of computeBounds() would be bounds with min > max.
    const auto &min = _bounds.getMin();
    const auto &max = _bounds.getMax();
    if (glm::max(min, max) == min)
    {
        _bounds = Bounds(Vector3f(0.f), Vector3f(0.f));
    }
}

Model &ModelInstance::getModel() noexcept
{
    return _model;
}

const Model &ModelInstance::getModel() const noexcept
{
    return _model;
}

const Metadata &ModelInstance::getModelMetadata() const noexcept
{
    auto &components = _model.getComponents();
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
    if (_transform != transform)
    {
        _transform = transform;
        auto affine = MatrixConverter::glmToOspray(_transform.toMatrix());
        _osprayInstance.setParam(InstanceParameters::transform, affine);
        computeBounds();
        _flag = true;
    }
}

const Transform &ModelInstance::getTransform() const noexcept
{
    return _transform;
}

const ospray::cpp::Instance &ModelInstance::getOsprayInstance() const noexcept
{
    return _osprayInstance;
}

bool ModelInstance::commit()
{
    if (!_flag)
    {
        return false;
    }

    _osprayInstance.commit();
    _flag = false;
    return true;
}
}
