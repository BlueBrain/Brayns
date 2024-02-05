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

#include "FrameExporter.h"

#include <brayns/common/Log.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/utils/Filesystem.h>
#include <brayns/utils/image/ImageCodecRegistry.h>
#include <brayns/utils/image/ImageEncoder.h>

namespace brayns
{
namespace
{
void checkExportParameters(const FrameExporter::ExportInfo& input)
{
    if (input.keyFrames.empty())
        throw FrameExportParameterException("No keyframe information");

    if (input.numSamples == 0)
        throw FrameExportParameterException(
            "Number of samples must be greater than 0");

    const auto& path = input.storePath;
    const auto bPath = fs::path(path);
    if (!fs::exists(bPath))
        throw FrameExportParameterException("Frame result path " + path +
                                            " not found");

    // Filesystem allow to query permissions, but no information about
    // current user permissions. To check wether we can write on the target
    // path without using system API, we attempt to create a directory and
    // capture any exception that may be thrown
    std::string testDirBaseName = "test_directory_";
    uint32_t indexTest{0u};
    while (fs::exists(fs::path(testDirBaseName + std::to_string(indexTest))))
        ++indexTest;

    const auto testDirName = testDirBaseName + std::to_string(indexTest);
    const fs::path testDirFilename(testDirName);
    try
    {
        fs::create_directory(bPath / testDirFilename);
    }
    catch (const fs::filesystem_error& fse)
    {
        throw FrameExportParameterException("Cannot write on " + path + ": " +
                                            std::string(fse.what()));
    }
    // Remove test directory on success
    fs::remove(bPath / testDirFilename);

    auto& format = input.imageFormat;
    if (!ImageCodecRegistry::isSupported(format))
    {
        throw FrameExportParameterException("Unknown image file format " +
                                            format);
    }
}
} // namespace

void FrameExporter::startNewExport(ExportInfo&& input)
{
    if (_exportRunning || _exportRequested)
        throw FrameExportInProgressException();

    checkExportParameters(input);

    _currentExport = std::move(input);
    _exportRequested = true;

    brayns::Log::info(
        "Export frame request\n"
        "- Number of frames:\t{}\n"
        "- Samples per pixel:\t{}\n"
        "- Export folder:\t{}",
        _currentExport.keyFrames.size(), _currentExport.numSamples,
        _currentExport.storePath);
}

void FrameExporter::preRender(Camera& camera, Renderer& renderer,
                              FrameBuffer& frameBuffer,
                              ParametersManager& parameters)
{
    if (_exportRequested)
    {
        _exportRunning = true;
        _exportRequested = false;
        _start(camera, renderer, frameBuffer, parameters);
    }

    if (!_exportRunning)
        return;

    // Finish on error
    if (_currentExportError)
    {
        _stop(camera, renderer, parameters);
        return;
    }

    // Export completed
    if (_currentExportKeyFrameIndex >= _currentExport.keyFrames.size())
    {
        _stop(camera, renderer, parameters);
        return;
    }

    // New frame begin, update camera and simulation
    if (_currentExportFrameAccumulation == 0)
    {
        const auto& keyFrame =
            _currentExport.keyFrames[_currentExportKeyFrameIndex];

        camera = keyFrame.camera;
        camera.setProperties(keyFrame.cameraParameters);
        parameters.getAnimationParameters().setFrame(keyFrame.frameIndex);
    }
}

void FrameExporter::postRender(FrameBuffer& frameBuffer)
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
        _writeImageToDisk(frameBuffer, frameNumberName);

        _currentExportFrameAccumulation = 0;
        ++_currentExportKeyFrameIndex;
    }
}

double FrameExporter::getExportProgress()
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

void FrameExporter::_saveState(Camera& camera, ParametersManager& parameters)
{
    _originalState = {};

    const auto& renderParams = parameters.getRenderingParameters();
    _originalState.hasAccumulation = renderParams.getAccumulation();
    _originalState.accumulationSize = renderParams.getMaxAccumFrames();

    _originalState.camera = camera;
    _originalState.cameraProperties = camera.getPropertyMap();
}

void FrameExporter::_restoreState(Camera& camera, ParametersManager& parameters)
{
    auto& renderParams = parameters.getRenderingParameters();
    renderParams.setAccumulation(_originalState.hasAccumulation);
    renderParams.setMaxAccumFrames(_originalState.accumulationSize);

    camera = _originalState.camera;
    camera.setProperties(_originalState.cameraProperties);

    camera.markModified();
    camera.commit();
}

void FrameExporter::_start(Camera& camera, Renderer& renderer,
                           FrameBuffer& frameBuffer,
                           ParametersManager& parameters) noexcept
{
    // Initialize export status
    _exportRunning = true;
    _currentExportKeyFrameIndex = 0u;
    _currentExportFrameAccumulation = 0u;
    _currentExportError = 0;
    _currentExportErrorMessage = "";

    _saveState(camera, parameters);

    auto& renderParams = parameters.getRenderingParameters();
    renderParams.setMaxAccumFrames(_currentExport.numSamples + 1);

    renderer.markModified();
    renderer.commit();

    frameBuffer.clear();
}

void FrameExporter::_stop(Camera& camera, Renderer& renderer,
                          ParametersManager& parameters) noexcept
{
    _exportRunning = false;
    _currentExportKeyFrameIndex = 0u;
    _currentExportFrameAccumulation = 0u;

    _restoreState(camera, parameters);

    renderer.markModified();
    renderer.commit();
}

void FrameExporter::_writeImageToDisk(FrameBuffer& frameBuffer,
                                      const uint32_t frameNumberName)
{
    auto image = frameBuffer.getImage();

    char frame[7];
    sprintf(frame, "%05d", static_cast<int32_t>(frameNumberName));
    auto& basePath = _currentExport.storePath;
    auto& extension = _currentExport.imageFormat;
    auto filename = basePath + '/' + frame + "." + extension;
    auto quality = _currentExport.imageQuality;

    try
    {
        ImageEncoder::save(image, filename, quality);
    }
    catch (const std::runtime_error& e)
    {
        _currentExportError = true;
        _currentExportErrorMessage = e.what();
        brayns::Log::error("{}", _currentExportErrorMessage);
        return;
    }

    frameBuffer.clear();

    brayns::Log::info("Frame saved to {}", filename);
}
} // namespace brayns
