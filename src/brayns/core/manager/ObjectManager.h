/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <concepts>
#include <mutex>

#include <brayns/core/utils/Logger.h>

#include "ObjectRegistry.h"

namespace brayns
{
class ObjectManager
{
public:
    explicit ObjectManager(ObjectRegistry objects, Logger &logger):
        _objects(std::move(objects)),
        _logger(&logger)
    {
    }

    auto visit(std::invocable<ObjectRegistry &> auto &&callable) -> decltype(callable(std::declval<ObjectRegistry &>()))
    {
        _logger->info("Waiting for object manager lock");
        auto lock = std::lock_guard(_mutex);
        _logger->info("Object manager lock acquired");

        return callable(_objects);
    }

private:
    std::mutex _mutex;
    ObjectRegistry _objects;
    Logger *_logger;
};
}
