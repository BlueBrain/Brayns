/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <stdexcept>

#include <brayns/engine/Camera.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
/**
 * @brief Exception thrown when a frame export request does not have any
 * keyframe information
 */
class FrameExportParameterException : public std::runtime_error
{
public:
    FrameExportParameterException(const std::string& msg)
        : std::runtime_error(msg)
    {
    }
};

/**
 * @brief Exception thrown when a export is in progress and a new one is
 * requested
 */
class FrameExportInProgressException : public std::runtime_error
{
public:
    FrameExportInProgressException()
        : std::runtime_error("")
    {
    }
};

/**
 * @brief Exception thrown when there is not a export in progress and a progress
 * report is requested
 */
class FrameExportNotRunningException : public std::runtime_error
{
public:
    FrameExportNotRunningException()
        : std::runtime_error("")
    {
    }
};

/**
 * @brief The FrameExportManager class manages frame export to disk requests
 * made through the CircutiExplorer plugin
 */
class FrameExporter
{
public:
    struct KeyFrame
    {
        uint64_t frameIndex;
        Camera camera;
        PropertyMap cameraParameters;
    };

    struct ExportInfo
    {
        std::string storePath;
        std::string imageFormat;
        uint32_t imageQuality;
        uint32_t numSamples;
        bool nameImageAfterSimulationFrameIndex;
        std::vector<KeyFrame> keyFrames;
    };

public:
    void startNewExport(ExportInfo&& input);

    void preRender(Camera& camera, Renderer& renderer, FrameBuffer& frameBuffer,
                   ParametersManager& parameters);
    void postRender(FrameBuffer& frameBuffer);

    double getExportProgress();

private:
    void _saveState(Camera& camera, ParametersManager& parameters);
    void _restoreState(Camera& camera, ParametersManager& parameters);

    void _start(Camera& camera, Renderer& renderer, FrameBuffer& framebuffer,
                ParametersManager& parameters) noexcept;
    void _stop(Camera& camera, Renderer& renderer,
               ParametersManager& parameters) noexcept;
    void _writeImageToDisk(FrameBuffer& frameBuffer,
                           const uint32_t frameNumberName);

private:
    struct OriginalState
    {
        bool hasAccumulation{false};
        uint32_t accumulationSize{0u};
        Camera camera;
        PropertyMap cameraProperties;
    };

    OriginalState _originalState;

    // Flags when an export is requested after the previous preRender()
    bool _exportRequested{false};
    bool _exportRunning{false};
    ExportInfo _currentExport{};
    uint64_t _currentExportKeyFrameIndex{0u};
    uint32_t _currentExportFrameAccumulation{0u};
    bool _currentExportError{false};
    std::string _currentExportErrorMessage{};
};
} // namespace brayns
