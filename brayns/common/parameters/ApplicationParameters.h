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

#pragma once

#include <brayns/common/Log.h>
#include <brayns/common/MathTypes.h>
#include <brayns/common/parameters/AbstractParameters.h>

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
    ApplicationParameters();

    /**
     * @brief print prints these parameters on the log
     */
    void print() final;

    /**
     * @brief getPlugins return the list of plugis with which brayns was
     * initialized
     * @return const std::vector<std::string>&
     */
    const std::vector<std::string> &getPlugins() const noexcept;

    /**
     * @brief Returns the system's log level
     *
     * @return LogLevel
     */
    LogLevel getLogLevel() const noexcept;

    /**
     * @brief getWindowSize returns the viewport size (width, height)
     * @return const Vector2ui&
     */
    const Vector2ui &getWindowSize() const noexcept;

    /**
     * @brief setWindowSize sets the viewport size (width, height)
     * @param size const Vector2ui&
     */
    void setWindowSize(const Vector2ui &size) noexcept;

    /**
     * @brief getJpegQuality return the stream JPEG quality.
     * @return int JPEG quality from 0 (lowest) to 100 (highest).
     */
    int getJpegQuality() const noexcept;

    /**
     * @brief setJpegQuality set the stream JPEG quality.
     * @param quality int JPEG quality from 0 (lowest) to 100 (highest).
     */
    void setJpegQuality(int quality) noexcept;

    /**
     * @brief posArgs return the positional arguments object used to initialize
     * this object
     * @return po::positional_options_description&
     */
    po::positional_options_description &posArgs() noexcept;

protected:
    /**
     * @brief parse parses the input parameters to initialize this object
     * @param vm const po::variables_map&
     */
    void parse(const po::variables_map &vm) final;

private:
    std::vector<std::string> _plugins;
    LogLevel _logLevel = LogLevel::Info;
    Vector2ui _windowSize;
    int _jpegQuality;
    po::positional_options_description _positionalArgs;
};
} // namespace brayns
