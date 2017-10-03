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
    void setWindowSize(const Vector2ui& size)
    {
        updateValue(_windowSize, size);
    }
    /** camera name (e.g. Perspective, Stereo, etc. ) */
    std::string getCamera() const { return _camera; }
    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }
    /** JPEG compression quality */
    void setJpegCompression(const size_t compression)
    {
        updateValue(_jpegCompression, compression);
    }
    size_t getJpegCompression() const { return _jpegCompression; }
    /** JPEG size */
    const Vector2ui& getJpegSize() const { return _jpegSize; }
    void setJpegSize(const Vector2ui& size) { updateValue(_jpegSize, size); }
    const strings& getFilters() const { return _filters; }
    void setFrameExportFolder(const std::string& folder)
    {
        updateValue(_frameExportFolder, folder);
    }
    std::string getFrameExportFolder() const { return _frameExportFolder; }
    /** Streaming enabled */
    bool getStreamingEnabled() const { return _streamingEnabled; }
    void setStreamingEnabled(const bool enabled)
    {
        updateValue(_streamingEnabled, enabled);
    }

    /** Stream compression enabled */
    bool getStreamCompression() const { return _streamCompression; }
    void setStreamCompression(const bool enabled)
    {
        updateValue(_streamCompression, enabled);
    }

    /** Stream compression quality, 1 (worst) to 100 (best) */
    unsigned int getStreamQuality() const { return _streamQuality; }
    void setStreamQuality(const unsigned int quality)
    {
        updateValue(_streamQuality, quality);
    }

    /** Stream ID; defaults to DEFLECT_ID if empty */
    const std::string& getStreamId() const { return _streamId; }
    void setStreamId(const std::string& id) { updateValue(_streamId, id); }
    /** Stream hostname; defaults to DEFLECT_HOST if empty */
    const std::string& getStreamHostname() const { return _streamHostname; }
    void setStreamHost(const std::string& host)
    {
        updateValue(_streamHostname, host);
    }

    /** Stream port; defaults to 1701 if empty */
    uint16_t getStreamPort() const { return _streamPort; }
    void setStreamPort(const uint16_t port) { updateValue(_streamPort, port); }
    /** Folder used by the application to store temporary files */
    std::string getTmpFolder() const { return _tmpFolder; }
    /** @return true if synchronous mode is enabled, aka rendering waits for
     * data loading. */
    bool getSynchronousMode() const { return _synchronousMode; }
    void setSynchronousMode(const bool synchronousMode)
    {
        updateValue(_synchronousMode, synchronousMode);
    }

protected:
    bool _parse(const po::variables_map& vm) final;

    std::string _camera;
    Vector2ui _windowSize;
    bool _benchmarking;
    size_t _jpegCompression;
    Vector2ui _jpegSize;
    strings _filters;
    std::string _frameExportFolder;
    bool _streamingEnabled{false};
    bool _streamCompression{true};
    unsigned int _streamQuality{80};
    uint16_t _streamPort{1701};
    std::string _streamHostname;
    std::string _streamId;
    std::string _tmpFolder;
    bool _synchronousMode{false};
};
}

#endif // APPLICATIONPARAMETERS_H
