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

size_t Model::getSizeInBytes() const noexcept
{
    return sizeof(Model) + _components.getSizeInBytes();
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
