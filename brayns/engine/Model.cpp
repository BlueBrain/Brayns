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

#include <brayns/engine/Model.h>

#include <ospray/SDK/common/OSPCommon.h>

namespace
{
auto glmMatrixToAffine(const brayns::Matrix4f& transform) noexcept
{
    const auto& strafe = transform[0];
    const auto& up = transform[1];
    const auto& forward = transform[2];
    const auto& position = transform[3];

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
Model::Model()
{
    _groupHandle = ospNewGroup();
}

Model::~Model()
{
    ospRelease(_groupHandle);
}

void Model::setMetaData(Metadata metadata) noexcept
{
    _metadata = std::move(metadata);
}

const Model::Metadata& Model::getMetaData() const noexcept
{
    return _metadata;
}

ModelComponents &Model::getComponents() noexcept
{
    return _components;
}

void Model::onPreRender(const AnimationParameters &animation)
{
    (void) animation;
}

void Model::onPostRender()
{
}

void Model::onRemoved()
{
}

void Model::setBoundsChanged(const bool val) noexcept
{
    _boundsChanged = val;
}

OSPGroup Model::groupHandle() const noexcept
{
    return _groupHandle;
}

ModelInstance::ModelInstance(const size_t modelID, Model* model)
 : _modelID(modelID)
 , _model(model)
{
    if(!_model)
        throw std::invalid_argument("A ModelInstance cannot be initialized with a null model");

    _instanceHandle = ospNewInstance(model->groupHandle());
    _recomputeBounds();
}

ModelInstance::~ModelInstance()
{
    ospRelease(_instanceHandle);
}

uint32_t ModelInstance::getID() const noexcept
{
    return _modelID;
}

const Bounds &ModelInstance::getBounds() const noexcept
{
    return _bounds;
}

void ModelInstance::commit()
{
    // Commit model data (If any other instance did it before, it will have no effect)
    _model->doCommit();

    // Recompute bounds if needed
    if(_model->_boundsChanged)
        _recomputeBounds();

    // Re-commit transform if needed
    if(_transformation.isModified())
    {
        const auto affine = glmMatrixToAffine(_transformation.toMatrix());
        ospSetParam(_instanceHandle, "transform", OSPDataType::OSP_AFFINE3F, &affine);
        ospCommit(_instanceHandle);
        _transformation.resetModified();
    }
}

Model& ModelInstance::getModel() noexcept
{
    // The caller could modify the model, so we must check on the next commit() call
    markModified(false);
    return *_model;
}

const Model& ModelInstance::getModel() const noexcept
{
    return *_model;
}

const Model::Metadata &ModelInstance::getModelMetadata() const noexcept
{
    return _model->getMetaData();
}

void ModelInstance::setVisible(const bool val) noexcept
{
    _updateValue(_visible, val);
}

bool ModelInstance::isVisible() const noexcept
{
    return _visible;
}

void ModelInstance::setTransform(const Transformation &transform) noexcept
{
    _updateValue(_transformation, transform);

    // Recompute bounds as soon as the transform changes
    if(_transformation.isModified())
        _recomputeBounds();
}

const Transformation& ModelInstance::getTransform() const noexcept
{
    return _transformation;
}

OSPInstance ModelInstance::handle() const noexcept
{
    return _instanceHandle;
}

void ModelInstance::_recomputeBounds() noexcept
{
    const auto matrix = _transformation.toMatrix();
    _bounds = _model->computeBounds(matrix);
    _boundsDirty = true;
}
} // namespace brayns
