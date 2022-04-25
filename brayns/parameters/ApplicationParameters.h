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
#include <brayns/parameters/AbstractParameters.h>

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
     * @brief setJpegCompression sets the JPEG image streamming compression
     * parameter
     * @param compression size_t
     */
    void setJpegCompression(const size_t compression) noexcept;

    /**
     * @brief getJpegCompression returns the JPEG image streamming compression
     * parameter
     * @return size_t
     */
    size_t getJpegCompression() const noexcept;

    /**
     * @brief getImageStreamFPS sets the max FPS at which the system should
     * stream frames to the connected clients
     * @return size_t
     */
    size_t getImageStreamFPS() const noexcept;

    /**
     * @brief setImageStreamFPS sets the max FPS at which the system should
     * stream frames to the connected clients
     * @param fps size_t
     */
    void setImageStreamFPS(const size_t fps) noexcept;

    /**
     * @brief getUseQuantaRenderControl returns whether quanta stream control
     * is enabled or not. Quanta stream control is the mode in which images are
     * only streamed to the clients upon explicit request
     * @return bool
     */
    bool getUseQuantaRenderControl() const noexcept;

    /**
     * @brief setUseQuantaRenderControl sets whether quanta stream control
     * is enabled or not. Quanta stream control is the mode in which images are
     * only streamed to the clients upon explicit request
     * @param value bool
     */
    void setUseQuantaRenderControl(const bool value) noexcept;

    /**
     * @brief Returns the system's log level
     *
     * @return LogLevel
     */
    LogLevel getLogLevel() const noexcept;

    /**
     * @brief getPlugins return the list of plugis with which brayns was
     * initialized
     * @return const std::vector<std::string>&
     */
    const std::vector<std::string> &getPlugins() const noexcept;

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

    Vector2ui _windowSize;
    size_t _jpegCompression;
    size_t _imageStreamFPS{60};
    bool _useQuantaRenderControl{false};
    LogLevel _logLevel;

    std::vector<std::string> _plugins;

    po::positional_options_description _positionalArgs;
};
} // namespace brayns
