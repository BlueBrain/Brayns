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

#include "FrameExportManager.h"

#include <brayns/common/Log.h>

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>

#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>

namespace
{
auto getFIImageFormat(const std::string& format) noexcept
{
    return format == "jpg" ? FIF_JPEG
                           : FreeImage_GetFIFFromFormat(format.c_str());
}

void checkExportParameters(const FrameExportManager::ExportInfo& input)
{
    if (input.keyFrames.empty())
        throw FrameExportParameterException("No keyframe information");

    if (input.numSamples == 0)
        throw FrameExportParameterException(
            "Number of samples must be greater than 0");

    const auto& path = input.storePath;
    const auto bPath = boost::filesystem::path(path);
    if (!boost::filesystem::exists(bPath))
        throw FrameExportParameterException("Frame result path " + path +
                                            " not found");

    const auto status = boost::filesystem::status(bPath);
    if (!(status.permissions() & boost::filesystem::perms::others_write))
        throw FrameExportParameterException("No write permissions on " + path);

    const auto imageFormat = getFIImageFormat(input.imageFormat);
    if (imageFormat == FIF_UNKNOWN)
        throw FrameExportParameterException("Unknown image file format " +
                                            input.imageFormat);
}
} // namespace

bool FrameExportManager::_exportRunning = false;
FrameExportManager::ExportInfo FrameExportManager::_currentExport = {};
uint64_t FrameExportManager::_currentExportKeyFrameIndex = 0u;
uint32_t FrameExportManager::_currentExportFrameAccumulation = 0u;
bool FrameExportManager::_currentExportError = false;
std::string FrameExportManager::_currentExportErrorMessage = {};

bool FrameExportManager::_originalAccumulationSetting = false;
uint32_t FrameExportManager::_originalAccumulationSize = 0u;

void FrameExportManager::startNewExport(brayns::PluginAPI& api,
                                        ExportInfo&& input)
{
    if (_exportRunning)
        throw FrameExportInProgressException();

    checkExportParameters(input);

    _currentExport = std::move(input);
    _start(api);

    brayns::Log::info(
        "[CE] -----------------------------------------------------------");
    brayns::Log::info("[CE] Movie settings     :");
    brayns::Log::info("[CE] - Number of frames : {}",
                      _currentExport.keyFrames.size());
    brayns::Log::info("[CE] - Samples per pixel: {}",
                      _currentExport.numSamples);
    brayns::Log::info("[CE] - Frame size       : {}",
                      api.getEngine().getFrameBuffer().getSize());
    brayns::Log::info("[CE] - Export folder    : {}", _currentExport.storePath);
    brayns::Log::info(
        "[CE] -----------------------------------------------------------");
}

void FrameExportManager::preRender(brayns::PluginAPI& api)
{
    if (!_exportRunning)
        return;

    // Finish on error
    if (_currentExportError)
    {
        _stop(api);
        return;
    }

    // Export completed
    if (_currentExportKeyFrameIndex >= _currentExport.keyFrames.size())
    {
        _stop(api);
        return;
    }

    // New frame begin, update camera and simulation
    if (_currentExportFrameAccumulation == 0)
    {
        const auto& keyFrame =
            _currentExport.keyFrames[_currentExportKeyFrameIndex];
        CameraUtils::updateCamera(api.getCamera(), keyFrame.camera);
        api.getParametersManager().getAnimationParameters().setFrame(
            keyFrame.frameIndex);
    }
}

void FrameExportManager::postRender(brayns::PluginAPI& api)
{
    if (!_exportRunning)
        return;

    ++_currentExportFrameAccumulation;
    // Frame completed, write to disk
    if (_currentExportFrameAccumulation >= _currentExport.numSamples)
    {
        auto frameNumberName = _currentExportKeyFrameIndex;
        if (_currentExport.nameImageAfterSimulationFrameIndex)
            frameNumberName =
                _currentExport.keyFrames[_currentExportKeyFrameIndex]
                    .frameIndex;
        _writeImageToDisk(api, frameNumberName);

        _currentExportFrameAccumulation = 0;
        ++_currentExportKeyFrameIndex;
    }
}

double FrameExportManager::getExportProgress()
{
    if (!_exportRunning)
        throw FrameExportNotRunningException();

    if (_currentExportError)
        throw std::runtime_error(_currentExportErrorMessage);

    const auto numKeyframes = _currentExport.keyFrames.size();
    const auto spp = _currentExport.numSamples;

    const auto totalFrameCount = numKeyframes * spp;
    const auto progress =
        _currentExportKeyFrameIndex * spp + _currentExportFrameAccumulation;

    return static_cast<double>(progress) / static_cast<double>(totalFrameCount);
}

void FrameExportManager::_start(brayns::PluginAPI& api) noexcept
{
    // Initialize export status
    _exportRunning = true;
    _currentExportKeyFrameIndex = 0u;
    _currentExportFrameAccumulation = 0u;
    _currentExportError = 0;
    _currentExportErrorMessage = "";

    // Modify rendering engine
    _originalAccumulationSetting =
        api.getParametersManager().getRenderingParameters().getAccumulation();
    if (_originalAccumulationSetting)
        _originalAccumulationSize = api.getParametersManager()
                                        .getRenderingParameters()
                                        .getMaxAccumFrames();

    api.getRenderer().markModified();
    api.getRenderer().commit();

    auto& frameBuffer = api.getEngine().getFrameBuffer();
    frameBuffer.clear();

    api.getParametersManager().getRenderingParameters().setMaxAccumFrames(
        static_cast<size_t>(_currentExport.numSamples) + 1);

    // Update camera for first frame
    CameraUtils::updateCamera(api.getCamera(),
                              _currentExport.keyFrames[0].camera);
}

void FrameExportManager::_stop(brayns::PluginAPI& api) noexcept
{
    _exportRunning = false;
    _currentExportKeyFrameIndex = 0u;
    _currentExportFrameAccumulation = 0u;

    if (!_originalAccumulationSetting)
        api.getParametersManager().getRenderingParameters().setAccumulation(
            false);
    else if (_currentExport.numSamples != _originalAccumulationSize)
        api.getParametersManager().getRenderingParameters().setMaxAccumFrames(
            _originalAccumulationSize);

    api.getRenderer().markModified();
    api.getRenderer().commit();
}

void FrameExportManager::_writeImageToDisk(brayns::PluginAPI& api,
                                           const uint32_t frameNumberName)
{
    auto& frameBuffer = api.getEngine().getFrameBuffer();
    auto image = frameBuffer.getImage();

    const auto fif = getFIImageFormat(_currentExport.imageFormat);

    if (fif == FIF_JPEG)
        image.reset(FreeImage_ConvertTo24Bits(image.get()));

    int flags = _currentExport.imageQuality;
    if (fif == FIF_TIFF)
        flags = TIFF_NONE;

    brayns::freeimage::MemoryPtr memory(FreeImage_OpenMemory());

    FreeImage_SaveToMemory(fif, image.get(), memory.get(), flags);

    BYTE* pixels = nullptr;
    DWORD numPixels = 0;
    FreeImage_AcquireMemory(memory.get(), &pixels, &numPixels);

    char frame[7];
    sprintf(frame, "%05d", static_cast<int32_t>(frameNumberName));

    const auto& basePath = _currentExport.storePath;
    const auto& extension = _currentExport.imageFormat;
    const auto fileName = basePath + '/' + frame + "." + extension;
    std::ofstream file(fileName, std::ios_base::binary);
    if (!file.is_open())
    {
        _currentExportError = true;
        _currentExportErrorMessage =
            "Unable to create or open image file " + fileName;
        brayns::Log::info("[CE] {}.", _currentExportErrorMessage);
        return;
    }

    file.write((char*)pixels, numPixels);
    file.close();

    frameBuffer.clear();

    brayns::Log::info("[CE] Frame saved to {}.", fileName);
}
