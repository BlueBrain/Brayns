/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/MathTypes.h>

#include "AbstractParameters.h"
#include <deque>

namespace brayns
{
class AbstractParameters;

/** Manages rendering parameters
 */
class RenderingParameters : public AbstractParameters
{
public:
    RenderingParameters();

    /**
     * @brief print prints these parameters to Log
     */
    void print() final;

    /**
     * @brief getCurrentRenderer returns the name of the system's
     * current renderer
     * @return const std::string&
     */
    const std::string& getCurrentRenderer() const noexcept;

    /**
     * @brief setCurrentRenderer sets the current renderer used by the
     * system
     * @param renderer const std::string&
     */
    void setCurrentRenderer(const std::string& renderer) noexcept;

    /**
     * @brief getRenderers returns a list with the names of all available
     * renderers
     * @return const std::vector<std::string>&
     */
    const std::vector<std::string>& getRenderers() const noexcept;

    /**
     * @brief addRenderer adds a new renderer name to the list of available
     * renderers. If the name is already present, this function has no effect
     * @param renderer const std::string&
     */
    void addRenderer(const std::string& renderer) noexcept;

    /**
     * @brief getCurrentCamera returns the name of the current camera used by
     * the system
     * @return const std::string&
     */
    const std::string& getCurrentCamera() const noexcept;

    /**
     * @brief getCameras returns a list with the names of all available
     * cameras
     * @return const std::vector<std::string>&
     */
    const std::vector<std::string>& getCameras() const noexcept;

    /**
     * @brief addCamera adds a new camera name to the list of available
     * cameras. If the name is already present, this function has no effect
     * @param camera const std::string&
     */
    void addCamera(const std::string& camera) noexcept;

    /**
     * @brief getSamplesPerPixel returns the number of samples per pixel
     * currently set for the system's renderer
     * @return uint32_t
     */
    uint32_t getSamplesPerPixel() const noexcept;

    /**
     * @brief setSamplesPerPixel sets the number of samples per pixel
     * to use on the system's renderer. The input value is clampped
     * as value = max(1, value)
     * @param value const uint32_t
     */
    void setSamplesPerPixel(const uint32_t value) noexcept;

    /**
     * @brief getSubsampling returns the subsampling factor used by the system.
     * The subsampling factor is a number used to compute the subsampling
     * frame size as: subsample frame size = frame size / subsamping factor
     * @return uint32_t
     */
    uint32_t getSubsampling() const noexcept;

    /**
     * @brief setSubsampling sets the subsampling factor used by the system.
     * The subsampling factor is a number used to compute the subsampling
     * frame size as: subsample frame size = frame size / subsamping factor.
     * The value is clampped as value = max(1, value)
     * @param subsampling const uint32_t
     */
    void setSubsampling(const uint32_t subsampling) noexcept;

    /**
     * @brief getBackgroundColor returns the background color used by the
     * systems renderer as normalized RGB values.
     * @return const Vector3f&
     */
    const Vector3f& getBackgroundColor() const noexcept;

    /**
     * @brief setBackgroundColor sets the background color used by the
     * systems renderer as normalized RGB values
     * @param value const Vector3f&
     */
    void setBackgroundColor(const Vector3f& value) noexcept;

    /**
     * @brief getHeadLight returns wether headlight is enabled on the systems
     * renderer or not. Headlight is a directional light which is always
     * facing the same direction as the camera
     * @return bool
     */
    bool getHeadLight() const noexcept;

    /**
     * @brief setHeadLight sets whether the headlight should be enabled or not.
     * Headlight is a directional light which is always facing the same
     * direction as the camera
     * @param headLight bool
     */
    void setHeadLight(bool headLight) noexcept;

    /**
     * @brief getAccumulation returns wether the system's renderer is using
     * accumulation rendering or not. Accumulation rendering is the proccess
     * of integrate a final frame over the course of multiple rendering
     * commands.
     * @return bool
     */
    bool getAccumulation() const noexcept;

    /**
     * @brief setAccumulation sets wether the system's renderer should use
     * accumulation rendering or not. Accumulation rendering is the proccess
     * of integrate a final frame over the course of multiple rendering
     * commands.
     * @param accumulation bool
     */
    void setAccumulation(bool accumulation) noexcept;

    /**
     * @brief getMaxAccumFrames returns the number of frames over which
     * the accumulation rendering process is executed.
     * This parameter is only taken into account if getAccumulation() is
     * true.
     * @return size_t
     */
    size_t getMaxAccumFrames() const noexcept;

    /**
     * @brief setMaxAccumFrames sets the number of frames over which
     * the accumulation rendering process is executed.
     * This parameter is only taken into account if getAccumulation() is
     * true.
     * @param value size_t
     */
    void setMaxAccumFrames(const size_t value) noexcept;

    /**
     * @brief getVarianceThreshold returns the maximum threshold of variance
     * the system's renderer is allowed to produce. A value that exceeds
     * this threshold will cause an error on the renderer.
     * @return double.
     */
    double getVarianceThreshold() const noexcept;

    /**
     * @brief setVarianceThreshold sets the maximum threshold of variance
     * the system's renderer is allowed to produce. A value that exceeds
     * this threshold will cause an error on the renderer.
     * A negative value will disable the threshold check
     * @param value double.
     */
    void setVarianceThreshold(const double value) noexcept;

protected:
    void parse(const po::variables_map& vm) final;

    std::string _renderer{"basic"};
    std::vector<std::string> _renderers;
    std::string _camera{"perspective"};
    std::vector<std::string> _cameras;
    uint32_t _spp{1};
    uint32_t _subsampling{1};
    bool _accumulation{true};
    Vector3f _backgroundColor{0.f, 0.f, 0.f};
    bool _headLight{true};
    double _varianceThreshold{-1.};
    size_t _maxAccumFrames{10};
};
} // namespace brayns
