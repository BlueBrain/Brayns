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

#include "Model.h"

namespace brayns
{
Model::~Model()
{
    _components.onDestroyed();
}

void Model::setMetaData(std::map<std::string, std::string> metadata) noexcept
{
    _metadata = std::move(metadata);
}

const std::map<std::string, std::string> &Model::getMetaData() const noexcept
{
    return _metadata;
}

ModelGroup &Model::getGroup() noexcept
{
    return _group;
}

uint32_t Model::getID() const noexcept
{
    return _modelId;
}

Bounds Model::computeBounds(const Matrix4f &transform) const noexcept
{
    return _components.computeBounds(transform);
}

void Model::onPreRender(const ParametersManager &params)
{
    _components.onPreRender(params);
}

void Model::onPostRender(const ParametersManager &params)
{
    _components.onPostRender(params);
}

OSPGroup Model::groupHandle() const noexcept
{
    return _group.handle();
}

bool Model::commit()
{
    if (_components.commit())
    {
        _group.commit();
        return true;
    }

    return false;
}
} // namespace brayns
