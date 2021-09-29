/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#ifndef PBRTCAMERA_H
#define PBRTCAMERA_H

#include <brayns/engine/Camera.h>

#include <pbrtv2/core/camera.h>
#include <pbrtv2/core/film.h>
#include <pbrtv2/core/filter.h>

#include <string>

/*
PBRT Camera parameters by name (type):
 - COMMON:
    - "shutteropen"     : float, default 0.f
    - "shuterclose"     : float, default 1.f
    - "lensradius"      : float, default 0.f
    - "focaldistance"   : float, default 1e6
    - "frameaspectratio": float, default current fb widht / height
    - "screenwindow"    : float[4] (minx, maxx, miny, maxy) // NOT NEEDED, PBRT
COMPUTES IT FROM frameaspectratio


 - "perspective"
    - "fov"             : float, default 90.f
    - "halffov"         : float, default -1.f
 - "environment"
 - "orthographic"
 - "telecentric"
*/

namespace brayns
{
class PBRTCamera : public Camera
{
public:
    PBRTCamera() {}

    ~PBRTCamera()
    {
        if (_pbrtCamera)
            delete _pbrtCamera;
    }

    void commit() final;

    void manualCommit(const Vector2ui& resolution);

    const pbrt::Transform& getLookAtMatrix() const { return _worldToCamMatrix; }
    const pbrt::Transform& getInvLookAtMatrix() const
    {
        return _camToWorldMatrix;
    }

    pbrt::Camera* impl() { return _pbrtCamera; }
    const pbrt::Camera* impl() const { return _pbrtCamera; }
    pbrt::Film* getFilm() { return _pbrtCamera->film; }
    pbrt::Film* getFilm() const { return _pbrtCamera->film; }
    bool needsToRender() const { return _needsRender; }
    void setNeedsToRender(const bool v) const { _needsRender = v; }
    bool cameraChanged() const { return _cameraChanged || isModified(); }
    void setCameraChanged(const bool v) const { _cameraChanged = v; }

private:
    void _createPBRTCamera();
    pbrt::Filter* _createFilter();
    pbrt::Film* _createFilm();

    pbrt::ParamSet _camParamstoPbrtParams();
    pbrt::ParamSet _filterParamsToPbrtParams();
    pbrt::ParamSet _filmParamsToPbrtParams();

    // Why isn't this memory released?
    // -> The renderer releases the camera
    //  -> The camera releases the film
    //   -> The film releases the filter
    pbrt::Camera* _pbrtCamera{nullptr};

    pbrt::Transform _worldToCamMatrix;
    pbrt::Transform _camToWorldMatrix;

    brayns::Vector2ui _currentRes{16, 16};

    mutable bool _needsRender{true};
    mutable bool _cameraChanged{true};
};
} // namespace brayns

#endif
