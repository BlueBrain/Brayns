/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/engine/Scene.h>

#include <brayns/network/json/MessageAdapter.h>

#include "TransformationAdapter.h"

namespace brayns
{
class UpdateModelProxy
{
public:
    UpdateModelProxy() = default;

    UpdateModelProxy(const Scene& scene)
        : _scene(&scene)
    {
    }

    void setId(size_t id)
    {
        if (!_scene)
        {
            throw std::runtime_error("Internal error with invalid setup");
        }
        _model = _scene->getModel(id);
        if (!_model)
        {
            throw EntrypointException("Unknown model ID: '" +
                                      std::to_string(id) + "'");
        }
    }

    void setBoundingBox(bool enabled)
    {
        _throwIfInvalid();
        _model->setBoundingBox(enabled);
    }

    void setName(const std::string& name)
    {
        _throwIfInvalid();
        _model->setName(name);
    }

    void setTransformation(const Transformation& transformation)
    {
        _throwIfInvalid();
        _model->setTransformation(transformation);
    }

    void setVisible(bool visible)
    {
        _throwIfInvalid();
        _model->setVisible(visible);
    }

    void computeBounds()
    {
        _throwIfInvalid();
        _model->computeBounds();
    }

private:
    void _throwIfInvalid()
    {
        if (_model)
        {
            return;
        }
        throw std::runtime_error("Internal error during model parsing");
    }

    ModelDescriptorPtr _model;
    const Scene* _scene = nullptr;
};

BRAYNS_NAMED_ADAPTER_BEGIN(UpdateModelProxy, "UpdateModelParams")
BRAYNS_ADAPTER_SET("id", setId, "Model ID", Required())
BRAYNS_ADAPTER_SET("bounding_box", setBoundingBox, "Display model bounds")
BRAYNS_ADAPTER_SET("name", setName, "Model name")
BRAYNS_ADAPTER_SET("transformation", setTransformation, "Model transformation")
BRAYNS_ADAPTER_SET("visible", setVisible, "Model visibility")
BRAYNS_ADAPTER_END()
} // namespace brayns