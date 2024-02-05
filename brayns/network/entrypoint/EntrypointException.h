/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <stdexcept>
#include <string>

#include <brayns/json/Json.h>

namespace brayns
{
/**
 * @brief Exception used to throw during the execution of a request inside an
 * entrypoint.
 *
 * Contains the necessary info to build an error message and send it
 * to the client.
 *
 */
class EntrypointException : public std::runtime_error
{
public:
    /**
     * @brief Construct an exception with only a description (code = 0).
     *
     * @param message Error description.
     */
    EntrypointException(const std::string& message)
        : std::runtime_error(message)
    {
    }

    /**
     * @brief Construct an exception with a code and a description.
     *
     * @param code Error code.
     * @param message Error description.
     */
    EntrypointException(int code, const std::string& message,
                        const JsonValue& data = {})
        : std::runtime_error(message)
        , _code(code)
        , _data(data)
    {
    }

    /**
     * @brief Construct an exception with additional data.
     *
     * @tparam T Error data type.
     * @param code Error code.
     * @param message Error description.
     * @param data Error data.
     */
    template <typename T>
    EntrypointException(int code, const std::string& message, const T& data)
        : std::runtime_error(message)
        , _code(code)
        , _data(Json::serialize(data))
    {
    }

    /**
     * @brief Get the error code of the exception.
     *
     * @return int Error code.
     */
    int getCode() const { return _code; }

    /**
     * @brief Get additional info about the error.
     *
     * @return const JsonValue& Error data.
     */
    const JsonValue& getData() const { return _data; }

private:
    int _code = 0;
    JsonValue _data;
};
} // namespace brayns
