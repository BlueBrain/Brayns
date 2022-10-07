/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "AtlasFactory.h"

#include "atlases/FlatmapAtlas.h"
#include "atlases/OrientationAtlas.h"
#include "atlases/ScalarAtlas.h"

namespace
{
template<typename T>
class DefaultFactory
{
public:
    static std::shared_ptr<Atlas>
        create(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &data)
    {
        return std::make_shared<T>(size, spacing, data);
    }
};
}

AtlasFactory AtlasFactory::createDefault()
{
    auto factories = AtlasFactory::Factories();
    factories[VoxelType::flatmap] = DefaultFactory<FlatmapAtlas>::create;
    factories[VoxelType::orientation] = DefaultFactory<OrientationAtlas>::create;
    factories[VoxelType::scalar] = DefaultFactory<ScalarAtlas>::create;
    return AtlasFactory(std::move(factories));
}

AtlasFactory::AtlasFactory(Factories factories)
    : _factories(std::move(factories))
{
}

std::shared_ptr<Atlas> AtlasFactory::create(
    VoxelType type,
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    const IDataMangler &data) const
{
    auto it = _factories.find(type);
    if (it == _factories.end())
    {
        throw std::invalid_argument("Unhandled voxel type");
    }

    auto fn = it->second;
    return fn(size, spacing, data);
}
