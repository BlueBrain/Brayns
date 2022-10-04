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

#pragma once

#include "DataMangler.h"
#include "IVoxelList.h"
#include "VoxelType.h"

#include <functional>
#include <memory>
#include <unordered_map>

class VoxelFactory
{
public:
    static VoxelFactory createDefault();

public:
    template<typename T>
    void registerType()
    {
        _factories[T::type] = std::make_unique<VoxelListFactory<T>>();
    }

    std::unique_ptr<IVoxelList> create(VoxelType type, const IDataMangler &mangler);

private:
    class IVoxelListFactory
    {
    public:
        virtual ~IVoxelListFactory() = default;
        virtual std::unique_ptr<IVoxelList> create(const IDataMangler &mangler) = 0;
    };

    template<typename T>
    class VoxelListFactory : public IVoxelListFactory
    {
    public:
        std::unique_ptr<IVoxelList> create(const IDataMangler &mangler) override
        {
            return std::make_unique<T>(mangler);
        }
    };

    std::unordered_map<VoxelType, std::unique_ptr<IVoxelListFactory>> _factories;
};
