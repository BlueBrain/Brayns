/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/pluginapi/PluginAPI.h>

#include <plugin/api/CameraUtils.h>
#include <plugin/network/messages/ExportFramesToDiskMessage.h>

// TODO: Frame export should be a core feature, not a plugin add-on. MOVE TO
// CORE
// ==============================================================================

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
class FrameExportManager
{
public:
    struct KeyFrame
    {
        uint64_t frameIndex;
        ODUCameraInformation camera;
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
    static void startNewExport(brayns::PluginAPI& api, ExportInfo&& input);

    static void preRender(brayns::PluginAPI& api);
    static void postRender(brayns::PluginAPI& api);

    static double getExportProgress();

private:
    static void _start(brayns::PluginAPI& api) noexcept;
    static void _stop(brayns::PluginAPI& api) noexcept;
    static void _writeImageToDisk(brayns::PluginAPI& api,
                                  const uint32_t frameNumberName);

private:
    static bool _exportRunning;
    static ExportInfo _currentExport;
    static uint64_t _currentExportKeyFrameIndex;
    static uint32_t _currentExportFrameAccumulation;
    static bool _currentExportError;
    static std::string _currentExportErrorMessage;

    static bool _originalAccumulationSetting;
    static uint32_t _originalAccumulationSize;
};
