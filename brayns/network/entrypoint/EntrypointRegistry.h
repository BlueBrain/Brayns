/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <string>
#include <unordered_map>

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
/**
 * @brief Contains all registered entrypoints.
 *
 */
class EntrypointRegistry
{
public:
    /**
     * @brief Retrieve an entrypoint using its method name.
     *
     * @param method Entrypoint method (ex: "set-camera").
     * @return const EntrypointRef* Entrypoint or null if not registered.
     */
    EntrypointRef *find(const std::string &method);

    /**
     * @brief Add a new entrypoint.
     *
     * @param entrypoint Entrypoint implementing common interface.
     * @throw std::invalid_argument Method missing or already present.
     */
    void add(EntrypointRef entrypoint);

    /**
     * @brief Iterate over all registered entrypoints.
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor with signature void(const EntrypointRef &).
     */
    template<typename FunctorType>
    void forEach(FunctorType functor)
    {
        for (const auto &[method, entrypoint] : _entrypoints)
        {
            functor(entrypoint);
        }
    }

private:
    std::unordered_map<std::string, EntrypointRef> _entrypoints;
};
} // namespace brayns
