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
uint32_t Model::getID() const noexcept
{
    return _modelId;
}

Components &Model::getComponents() noexcept
{
    return _components;
}

const Components &Model::getComponents() const noexcept
{
    return _components;
}

InspectResult Model::inspect(const InspectContext &context)
{
    return _systems.inspect(context, _components);
}

Bounds Model::computeBounds(const Matrix4f &matrix)
{
    return _systems.computeBounds(matrix, _components);
}

void Model::init()
{
    _systems.init(_components);
}

void Model::onPreRender(const ParametersManager &parameters)
{
    _systems.preRender(parameters, _components);
}

CommitResult Model::commit()
{
    return _systems.commit(_components);
}

void Model::onPostRender(const ParametersManager &parameters)
{
    return _systems.postRender(parameters, _components);
}
} // namespace brayns
