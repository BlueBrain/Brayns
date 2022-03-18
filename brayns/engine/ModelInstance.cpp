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
auto glmMatrixToAffine(const brayns::Matrix4f &transform) noexcept
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
}

namespace brayns
{
ModelInstance::ModelInstance(const uint32_t modelInstanceID, Model& model)
    : _modelInstanceID(modelInstanceID)
    , _model(model)
{
    _instanceHandle = ospNewInstance(model.groupHandle());
    computeBounds();
}

ModelInstance::~ModelInstance()
{
    ospRelease(_instanceHandle);
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
    const auto matrix = _transformation.toMatrix();
    _bounds = _model.computeBounds(matrix);
}

Model &ModelInstance::getModel() noexcept
{
    return _model;
}

const Model &ModelInstance::getModel() const noexcept
{
    return _model;
}

const std::map<std::string, std::string> &ModelInstance::getModelMetadata() const noexcept
{
    return _model.getMetaData();
}

void ModelInstance::setVisible(const bool val) noexcept
{
    _visibilityChanged = _visible != val;
    _visible = val;
}

bool ModelInstance::isVisible() const noexcept
{
    return _visible;
}

void ModelInstance::setTransform(const Transformation &transform) noexcept
{
    _transformation = transform;
    // Recompute bounds as soon as the transform changes
    if (_transformation.isModified())
        computeBounds();
}

const Transformation &ModelInstance::getTransform() const noexcept
{
    return _transformation;
}

OSPInstance ModelInstance::handle() const noexcept
{
    return _instanceHandle;
}

bool ModelInstance::commit(const bool modelChanged)
{
    bool needsCommit = modelChanged;

    // Re-commit transform if needed
    if (_transformation.isModified())
    {
        const auto affine = glmMatrixToAffine(_transformation.toMatrix());
        ospSetParam(_instanceHandle, "transform", OSPDataType::OSP_AFFINE3F, &affine);
        _transformation.resetModified();
        needsCommit = true;
    }

    if (needsCommit)
    {
        ospCommit(_instanceHandle);
    }

    return needsCommit || _visibilityChanged;
}
}
