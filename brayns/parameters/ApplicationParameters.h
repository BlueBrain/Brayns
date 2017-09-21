/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef APPLICATIONPARAMETERS_H
#define APPLICATIONPARAMETERS_H

#include "AbstractParameters.h"

#include <brayns/common/types.h>
#include <string>
#include <vector>

namespace brayns
{
/** Manages application parameters
 */
class ApplicationParameters : public AbstractParameters
{
public:
    ApplicationParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** window size */
    const Vector2ui& getWindowSize() const { return _windowSize; }
    void setWindowSize(const Vector2ui& size) { _windowSize = size; }
    /** camera name (e.g. Perspective, Stereo, etc. ) */
    std::string getCamera() const { return _camera; }
    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }
    /** JPEG compression quality */
    void setJpegCompression(const size_t compression)
    {
        _jpegCompression = compression;
    }
    size_t getJpegCompression() const { return _jpegCompression; }
    /** JPEG size */
    const Vector2ui& getJpegSize() const { return _jpegSize; }
    void setJpegSize(const Vector2ui& size) { _jpegSize = size; }
    const strings& getFilters() const { return _filters; }
    /**
     * @brief Auto publication of ZeroEQ events is used when several
     * applications supporting the ZeroEQ protocol are started and need to
     * be remain synchronized.
     * @return True if auto publication is enabled, false otherwize
     */
    bool getAutoPublishZeroEQEvents() const { return _autoPublishZeroEQEvents; }
    void setFrameExportFolder(const std::string& folder)
    {
        _frameExportFolder = folder;
    }
    std::string getFrameExportFolder() const { return _frameExportFolder; }
    /** Streaming enabled */
    bool getStreamingEnabled() const { return _streamingEnabled; }
    void setStreamingEnabled(const bool enabled)
    {
        _streamingEnabled = enabled;
    }

    /** Stream compression enabled */
    bool getStreamCompression() const { return _streamCompression; }
    void setStreamCompression(const bool enabled)
    {
        _streamCompression = enabled;
    }

    /** Stream compression quality, 1 (worst) to 100 (best) */
    unsigned int getStreamQuality() const { return _streamQuality; }
    void setStreamQuality(const unsigned int quality)
    {
        _streamQuality = quality;
    }

    /** Folder used by the application to store temporary files */
    std::string getTmpFolder() const { return _tmpFolder; }
protected:
    bool _parse(const po::variables_map& vm) final;

    std::string _camera;
    Vector2ui _windowSize;
    bool _benchmarking;
    size_t _jpegCompression;
    Vector2ui _jpegSize;
    strings _filters;
    bool _autoPublishZeroEQEvents;
    std::string _frameExportFolder;
    bool _streamingEnabled{true};
    bool _streamCompression{true};
    unsigned int _streamQuality{80};
    std::string _tmpFolder;
};
}

#endif // APPLICATIONPARAMETERS_H
