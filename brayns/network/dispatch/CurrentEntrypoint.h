/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
/**
 * @brief Holds the current entrypoint being processed.
 *
 */
class CurrentEntrypoint
{
public:
    /**
     * @brief No current entrypoint.
     *
     */
    CurrentEntrypoint() = default;

    /**
     * @brief Set the current entrypoint.
     *
     * @param entrypoint Current entrypoint being processed.
     */
    CurrentEntrypoint(const EntrypointRef &entrypoint);

    /**
     * @brief Helper class to get the current method or an empty string.
     *
     * @return std::string Current entrypoint method or empty.
     */
    std::string getMethod() const;

    /**
     * @brief Check if a current entrypoint is set.
     *
     * @return true Current entrypoint being processed.
     * @return false No entrypoints being processed.
     */
    operator bool() const;

private:
    const EntrypointRef *_entrypoint = nullptr;
};
} // namespace brayns
