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

#include "PBRTCamera.h"
#include "PBRTConstants.h"
#include "util/Util.h"

#include <brayns/parameters/ParametersManager.h>

#include <pbrtv2/cameras/environment.h>
#include <pbrtv2/cameras/orthographic.h>
#include <pbrtv2/cameras/perspective.h>
#include <pbrtv2/cameras/telecentric.h>

#include <pbrtv2/film/image.h>

#include <pbrtv2/filters/box.h>
#include <pbrtv2/filters/gaussian.h>
#include <pbrtv2/filters/mitchell.h>
#include <pbrtv2/filters/sinc.h>
#include <pbrtv2/filters/triangle.h>

#include <pbrtv2/core/parallel.h>
#include <pbrtv2/core/paramset.h>
#include <pbrtv2/core/probes.h>
#include <pbrtv2/core/transform.h>

namespace brayns
{
inline bool isKnownCamera(const std::string& type)
{
    return type == PBRT_CAMTYPE_TELECENTRIC ||
           type == PBRT_CAMTYPE_ENVIRONMENT ||
           type == PBRT_CAMTYPE_ORTOGRAPHIC ||
           type == PBRT_CAMTYPE_PERSPECTIVE;
}

void PBRTCamera::commit()
{
    // Camera commit must be fully controlled by the renderer
    // Since it is the renderer object which manages the
    // camera memory in PBRT v2
}

void PBRTCamera::manualCommit(const Vector2ui& resolution)
{
    if(!isModified() && _currentRes == resolution)
        return;

    _needsRender = true;

    const auto& srcPos = getPosition();
    const pbrt::Point pos = glmToPbrt3<pbrt::Point>(srcPos);

    const auto srcDir = glm::rotate(getOrientation(), Vector3d(0., 0., -1.));

    const auto srcLook = srcPos + srcDir * 100.0;
    const pbrt::Point look = glmToPbrt3<pbrt::Point>(srcLook);

    const auto srcUp = glm::normalize(glm::rotate(getOrientation(), Vector3d(0., -1., 0.)));
    const pbrt::Vector up = glmToPbrt3<pbrt::Vector>(srcUp);

    _camToWorldMatrix = pbrt::LookAt(pos, look, up);
    _worldToCamMatrix = pbrt::Transform(_camToWorldMatrix.GetInverseMatrix(),
                                        _camToWorldMatrix.GetMatrix());

    if(!isKnownCamera(getCurrentType()))
    {
        BRAYNS_WARN << "PBRTCamera: Unknown camera type " << getCurrentType()
                    << ", falling back to Perspective" << std::endl;
        _currentType = PBRT_CAMTYPE_PERSPECTIVE;
    }
    else
        _currentType = getCurrentType();

    _currentRes = resolution;
    _createPBRTCamera();
    _cameraChanged =  true;
}

void PBRTCamera::_createPBRTCamera()
{
    // Since the camera will be modified,
    // The renderer rebuilt will be trigger, and it will release the memory
    // of the previous camera
    _pbrtCamera = nullptr;

    pbrt::AnimatedTransform atr (&_worldToCamMatrix, 0.f, &_worldToCamMatrix, 1.f);
    pbrt::ParamSet params = _camParamstoPbrtParams();
    pbrt::Film* film = _createFilm();

    if(getCurrentType() == PBRT_CAMTYPE_PERSPECTIVE)
        _pbrtCamera = pbrt::CreatePerspectiveCamera(params, atr, film);
    else if(getCurrentType() == PBRT_CAMTYPE_ORTOGRAPHIC)
        _pbrtCamera = pbrt::CreateOrthographicCamera(params, atr, film);
    else if(getCurrentType() == PBRT_CAMTYPE_ENVIRONMENT)
        _pbrtCamera = pbrt::CreateEnvironmentCamera(params, atr, film);
    else if (getCurrentType() == PBRT_CAMTYPE_TELECENTRIC)
        _pbrtCamera = pbrt::CreateTelecentricCamera(params, atr, film);

    if(_pbrtCamera == nullptr)
        throw std::runtime_error("PBRTCamera: Could not create PBRT Camera implementation");
}

pbrt::Filter* PBRTCamera::_createFilter()
{
    pbrt::Filter* filter = nullptr;
    if(hasProperty("filter_type"))
    {
        const auto params = _filterParamsToPbrtParams();

        const auto ft = getProperty<std::string>(PBRT_FILTERPROP_TYPE);
        if(ft == PBRT_FILTERTYPE_BOX)
            filter = pbrt::CreateBoxFilter(params);
        else if(ft == PBRT_FILTERTYPE_GAUSSIAN)
            filter = pbrt::CreateGaussianFilter(params);
        else if(ft == PBRT_FILTERTYPE_MITCHELL)
            filter = pbrt::CreateMitchellFilter(params);
        else if(ft == PBRT_FILTERTYPE_SINC)
            filter = pbrt::CreateSincFilter(params);
        else if(ft == PBRT_FILTERTYPE_TRIANGLE)
            filter = pbrt::CreateTriangleFilter(params);
        else
            BRAYNS_WARN << "PBRTCamera: Unknown filter type "
                        << ft
                        << ", falling to Box filter" << std::endl;
    }

    if(filter == nullptr)
        filter = pbrt::CreateBoxFilter(pbrt::ParamSet());

    return filter;
}

pbrt::Film* PBRTCamera::_createFilm()
{
    return pbrt::CreateImageFilm(_filmParamsToPbrtParams(), _createFilter());
}

inline void addFloat(const brayns::PropertyObject& src,
                     pbrt::ParamSet& dst,
                     const std::string& propName,
                     const double defaultValue)
{
    auto v = static_cast<float>(src.getPropertyOrValue<double>(propName, defaultValue));
    dst.AddFloat(propName, &v);
}

inline void addInt(const brayns::PropertyObject& src,
                   pbrt::ParamSet& dst,
                   const std::string& propName,
                   const int defaultValue)
{
    auto val = src.getPropertyOrValue<int>(propName, defaultValue);
    dst.AddInt(propName, &val, 1);
}

pbrt::ParamSet PBRTCamera::_camParamstoPbrtParams()
{
    pbrt::ParamSet result;

    // COMMON
    addFloat(*this, result, PBRT_CAMPROP_SHUTTEROPEN, 0.);
    addFloat(*this, result, PBRT_CAMPROP_SHUTTERCLOSE, 1.);
    addFloat(*this, result, PBRT_CAMPROP_LENSRADIUS, 0.);
    addFloat(*this, result, PBRT_CAMPROP_FOCALDISTANCE, 1e6);

    auto far = static_cast<float>(static_cast<double>(_currentRes.x) /
                                  static_cast<double>(_currentRes.y));
    result.AddFloat(PBRT_CAMPROP_FRAMEASPECTRATIO, &far);

    if(getCurrentType() == PBRT_CAMTYPE_PERSPECTIVE)
    {
        addFloat(*this, result, PBRT_CAMPROP_FOV, 90.);
        addFloat(*this, result, PBRT_CAMPROP_HALFFOV, -1.);
    }

    return result;
}

pbrt::ParamSet PBRTCamera::_filterParamsToPbrtParams()
{
    pbrt::ParamSet result;

    if(!hasProperty("filter_type"))
        return result;

    const auto filterType = getProperty<std::string>(PBRT_FILTERPROP_TYPE);

    if(filterType == PBRT_FILTERTYPE_BOX)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 0.5);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 0.5);
    }
    else if(filterType == PBRT_FILTERTYPE_GAUSSIAN)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_ALPHA, 2.);
    }
    else if(filterType == PBRT_FILTERTYPE_MITCHELL)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 2.);
        constexpr auto val = 1. / 3.;
        addFloat(*this, result, PBRT_FILTERPROP_B, val);
        addFloat(*this, result, PBRT_FILTERPROP_C, val);
    }
    else if(filterType == PBRT_FILTERTYPE_SINC)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 4.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 4.);
        addFloat(*this, result, PBRT_FILTERPROP_TAU, 3.);
    }
    else if(filterType == PBRT_FILTERTYPE_TRIANGLE)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 2.);
    }

    return result;
}

pbrt::ParamSet PBRTCamera::_filmParamsToPbrtParams()
{
    pbrt::ParamSet result;
    addInt(*this, result, PBRT_FILMPROP_XRESOLUTION, static_cast<int>(_currentRes.x));
    addInt(*this, result, PBRT_FILMPROP_YRESOLUTION, static_cast<int>(_currentRes.y));
    addFloat(*this, result, PBRT_FILMPROP_PIXELWIDTH, 1.);
    addFloat(*this, result, PBRT_FILMPROP_PIXELHEIGHT, 1.);
    
    auto cropWindow = getPropertyOrValue<std::array<double, 4>>(PBRT_FILMPROP_CROPWINDOW, 
                                                                {0., 1., 0., 1.});
    float cropWindowFlt[4] = {static_cast<float>(cropWindow[0]), 
                              static_cast<float>(cropWindow[1]), 
                              static_cast<float>(cropWindow[2]), 
                              static_cast<float>(cropWindow[3])};
    result.AddFloat(PBRT_FILMPROP_CROPWINDOW, &cropWindowFlt[0], 4);

    auto filterSpectrum = getPropertyOrValue<bool>(PBRT_FILMPROP_FILTERSPECTRUM, false);
    result.AddBool(PBRT_FILMPROP_FILTERSPECTRUM, &filterSpectrum, 1);

    auto filterMin = getPropertyOrValue<int>(PBRT_FILMPROP_FILTERBANDMIN, 400);
    result.AddInt(PBRT_FILMPROP_FILTERBANDMIN, &filterMin, 1);
    auto filterMax = getPropertyOrValue<int>(PBRT_FILMPROP_FILTERBANDMAX, 700);
    result.AddInt(PBRT_FILMPROP_FILTERBANDMAX, &filterMax, 1);

    return result;
}

}
