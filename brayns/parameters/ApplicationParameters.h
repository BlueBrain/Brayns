/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/parameters/AbstractParameters.h>
#include <brayns/utils/Log.h>
#include <brayns/utils/MathTypes.h>

#include <string>
#include <vector>

namespace brayns
{
/**
 * @brief The ApplicationParameters class manages application parameters
 */
class ApplicationParameters : public AbstractParameters
{
public:
    /**
     * @brief Returns the system's log level
     *
     * @return LogLevel Log level.
     */
    LogLevel getLogLevel() const noexcept;

    /**
     * @brief Returns the viewport size (width, height).
     * @return const Vector2ui& Viewport.
     */
    const Vector2ui &getWindowSize() const noexcept;

    /**
     * @brief Set the viewport size (width, height).
     * @param size const Vector2ui& New viewport.
     */
    void setWindowSize(const Vector2ui &size) noexcept;

    /**
     * @brief Register argv properties of the parameter set.
     *
     * @param builder Helper class to register argv properties.
     */
    virtual void build(ArgvBuilder &builder) override;

private:
    LogLevel _logLevel = LogLevel::Info;
    Vector2ui _windowSize = {800, 600};
};
} // namespace brayns
