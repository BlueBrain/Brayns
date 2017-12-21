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

SERIALIZATION_ACCESS(ApplicationParameters)

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
        _updateValue(_windowSize, size);
    }
    /** camera name (e.g. Perspective, Stereo, etc. ) */
    std::string getCamera() const { return _camera; }
    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }
    /** JPEG compression quality */
    void setJpegCompression(const size_t compression)
    {
        _updateValue(_jpegCompression, compression);
    }
    size_t getJpegCompression() const { return _jpegCompression; }
    /** JPEG size */
    const Vector2ui& getJpegSize() const { return _jpegSize; }
    void setJpegSize(const Vector2ui& size) { _updateValue(_jpegSize, size); }
    /** Image stream FPS */
    size_t getImageStreamFPS() const { return _imageStreamFPS; }
    void setImageStreamFPS(const size_t fps)
    {
        _updateValue(_imageStreamFPS, fps);
    }

    const strings& getFilters() const { return _filters; }
    void setFrameExportFolder(const std::string& folder)
    {
        _updateValue(_frameExportFolder, folder);
    }
    std::string getFrameExportFolder() const { return _frameExportFolder; }
    /** Folder used by the application to store temporary files */
    std::string getTmpFolder() const { return _tmpFolder; }
    /** @return true if synchronous mode is enabled, aka rendering waits for
     * data loading. */
    bool getSynchronousMode() const { return _synchronousMode; }
    void setSynchronousMode(const bool synchronousMode)
    {
        _updateValue(_synchronousMode, synchronousMode);
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
    std::string _tmpFolder;
    bool _synchronousMode{false};
    size_t _imageStreamFPS{60};

    SERIALIZATION_FRIEND(ApplicationParameters)
};
}

#endif // APPLICATIONPARAMETERS_H
