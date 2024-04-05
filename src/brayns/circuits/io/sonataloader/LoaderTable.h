/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include <brayns/circuits/io/sonataloader/populations/EdgePopulationLoader.h>
#include <brayns/circuits/io/sonataloader/populations/NodePopulationLoader.h>

#include <memory>
#include <string>
#include <vector>

namespace sonataloader
{
/**
 * @brief Utility class to hold loaders identified by their name
 *
 * @tparam T
 */
template<typename T>
class LoaderTable
{
public:
    template<typename TImpl>
    void registerLoader()
    {
        _loaders.push_back(std::make_unique<TImpl>());
    }

    const T &getLoader(const std::string &name) const
    {
        const auto begin = _loaders.begin();
        const auto end = _loaders.end();
        auto it = std::find_if(
            begin,
            end,
            [&](const std::unique_ptr<T> &loader)
            {
                const auto handledType = loader->getPopulationType();
                return handledType == name;
            });

        if (it == end)
        {
            throw std::invalid_argument("No population loader for type " + name);
        }

        return *(*it);
    }

private:
    std::vector<std::unique_ptr<T>> _loaders;
};

struct NodeLoaderTable
{
    static LoaderTable<NodePopulationLoader> create() noexcept;
};

struct EdgeLoaderTable
{
    static LoaderTable<EdgePopulationLoader> create() noexcept;
};
}
