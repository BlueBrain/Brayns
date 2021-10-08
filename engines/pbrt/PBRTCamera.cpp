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

#include <pbrt/cameras/environment.h>
#include <pbrt/cameras/orthographic.h>
#include <pbrt/cameras/perspective.h>
#include <pbrt/cameras/realistic.h>

#include <pbrt/filters/box.h>
#include <pbrt/filters/gaussian.h>
#include <pbrt/filters/mitchell.h>
#include <pbrt/filters/sinc.h>
#include <pbrt/filters/triangle.h>

#include <pbrt/core/film.h>
#include <pbrt/core/medium.h>
#include <pbrt/core/paramset.h>
#include <pbrt/core/transform.h>

#include <limits>

namespace brayns
{
inline bool isKnownCamera(const std::string& type)
{
    return type == PBRT_CAMTYPE_REALISTIC || type == PBRT_CAMTYPE_ENVIRONMENT ||
           type == PBRT_CAMTYPE_ORTOGRAPHIC || type == PBRT_CAMTYPE_PERSPECTIVE;
}

void PBRTCamera::commit()
{
    manualCommit(_currentRes);
}

void PBRTCamera::manualCommit(const Vector2ui& resolution)
{
    const auto typeChanged = (_currentType != getCurrentType());
    if (!isModified() && _currentRes == resolution && !typeChanged)
        return;

    const auto& srcPos = getPosition();
    const pbrt::Point3f pos = TO_PBRT_P3(srcPos);

    const auto srcDir = glm::rotate(getOrientation(), Vector3d(0., 0., -1.));

    const auto srcLook = srcPos + srcDir * 100.0;
    const pbrt::Point3f look = TO_PBRT_P3(srcLook);

    const auto srcUp =
        glm::normalize(glm::rotate(getOrientation(), Vector3d(0., -1., 0.)));
    const pbrt::Vector3f up = TO_PBRT_V3(srcUp);

    _camToWorldMatrix = pbrt::LookAt(pos, look, up);
    _worldToCamMatrix = pbrt::Transform(_camToWorldMatrix.GetInverseMatrix(),
                                        _camToWorldMatrix.GetMatrix());

    _currentType = getCurrentType();
    if (!isKnownCamera(_currentType))
    {
        BRAYNS_WARN << "PBRTCamera: Unknown camera type " << _currentType
                    << ", falling back to Perspective" << std::endl;
        _currentType = PBRT_CAMTYPE_PERSPECTIVE;
    }
    _currentRes = resolution;
    _createPBRTCamera();
    _cameraChanged = true;
}

void PBRTCamera::_createPBRTCamera()
{
    _pbrtCamera.reset();

    pbrt::AnimatedTransform atr(&_worldToCamMatrix, 0.f, &_worldToCamMatrix,
                                1.f);
    pbrt::ParamSet params = _camParamstoPbrtParams();

    _film = pbrt::CreateFilm(_filmParamsToPbrtParams(), _createFilter());
    pbrt::MediumInterface mockMedium;
    pbrt::Camera* result = nullptr;

    if (getCurrentType() == PBRT_CAMTYPE_PERSPECTIVE)
        result = pbrt::CreatePerspectiveCamera(params, atr, _film,
                                               mockMedium.outside);
    else if (getCurrentType() == PBRT_CAMTYPE_ORTOGRAPHIC)
        result = pbrt::CreateOrthographicCamera(params, atr, _film,
                                                mockMedium.outside);
    else if (getCurrentType() == PBRT_CAMTYPE_ENVIRONMENT)
        result = pbrt::CreateEnvironmentCamera(params, atr, _film,
                                               mockMedium.outside);
    else if (getCurrentType() == PBRT_CAMTYPE_REALISTIC)
        result =
            pbrt::CreateRealisticCamera(params, atr, _film, mockMedium.outside);

    if (result == nullptr)
        throw std::runtime_error(
            "PBRTCamera: Could not create PBRT Camera implementation");

    _pbrtCamera = std::shared_ptr<pbrt::Camera>(result);
}

std::unique_ptr<pbrt::Filter> PBRTCamera::_createFilter()
{
    pbrt::Filter* resultPtr = nullptr;
    if (hasProperty("filter_type"))
    {
        const auto params = _filterParamsToPbrtParams();

        const auto ft = getProperty<std::string>(PBRT_FILTERPROP_TYPE);
        if (ft == PBRT_FILTERTYPE_BOX)
            resultPtr = pbrt::CreateBoxFilter(params);
        else if (ft == PBRT_FILTERTYPE_GAUSSIAN)
            resultPtr = pbrt::CreateGaussianFilter(params);
        else if (ft == PBRT_FILTERTYPE_MITCHELL)
            resultPtr = pbrt::CreateMitchellFilter(params);
        else if (ft == PBRT_FILTERTYPE_SINC)
            resultPtr = pbrt::CreateSincFilter(params);
        else if (ft == PBRT_FILTERTYPE_TRIANGLE)
            resultPtr = pbrt::CreateTriangleFilter(params);
        else
            BRAYNS_WARN << "PBRTCamera: Unknown filter type " << ft
                        << ", falling to Box filter" << std::endl;
    }

    if (resultPtr == nullptr)
        resultPtr = pbrt::CreateBoxFilter(pbrt::ParamSet());

    return std::unique_ptr<pbrt::Filter>(resultPtr);
}

inline void addFloat(const brayns::PropertyObject& src, pbrt::ParamSet& dst,
                     const std::string& propName, const double defaultValue)
{
    std::unique_ptr<pbrt::Float[]> val(new pbrt::Float[1]);
    val.get()[0] = static_cast<pbrt::Float>(
        src.getPropertyOrValue<double>(propName, defaultValue));
    dst.AddFloat(propName, std::move(val));
}

inline void addInt(const brayns::PropertyObject& src, pbrt::ParamSet& dst,
                   const std::string& propName, const int defaultValue)
{
    std::unique_ptr<int[]> val(new int[1]);
    val.get()[0] = src.getPropertyOrValue<int>(propName, defaultValue);
    dst.AddInt(propName, std::move(val), 1);
}

pbrt::ParamSet PBRTCamera::_camParamstoPbrtParams()
{
    pbrt::ParamSet result;

    // COMMON
    addFloat(*this, result, PBRT_CAMPROP_SHUTTEROPEN, 0.);
    addFloat(*this, result, PBRT_CAMPROP_SHUTTERCLOSE, 1.);

    // SPECIFIC
    if (getCurrentType() == PBRT_CAMTYPE_PERSPECTIVE ||
        getCurrentType() == PBRT_CAMTYPE_ORTOGRAPHIC ||
        getCurrentType() == PBRT_CAMTYPE_ENVIRONMENT)
    {
        addFloat(*this, result, PBRT_CAMPROP_LENSRADIUS, 0.);
        addFloat(*this, result, PBRT_CAMPROP_FOCUSDISTANCE, 1e6);

        auto far = static_cast<double>(_currentRes.x) /
                   static_cast<double>(_currentRes.y);

        // addFloat(*this, result, PBRT_CAMPROP_FRAMEASPECTRATIO, far);
        std::unique_ptr<pbrt::Float[]> val(new pbrt::Float[1]);
        val.get()[0] = static_cast<pbrt::Float>(far);
        result.AddFloat(PBRT_CAMPROP_FRAMEASPECTRATIO, std::move(val));

        if (getCurrentType() == PBRT_CAMTYPE_PERSPECTIVE)
        {
            addFloat(*this, result, PBRT_CAMPROP_FOV, 90.);
            addFloat(*this, result, PBRT_CAMPROP_HALFFOV, -1.);
        }
    }
    else if (getCurrentType() == PBRT_CAMTYPE_REALISTIC)
    {
        std::unique_ptr<std::string[]> lensFileVal(new std::string[1]);
        lensFileVal.get()[0] =
            getPropertyOrValue<std::string>(PBRT_CAMPROP_LENSFILE, "");
        result.AddString(PBRT_CAMPROP_LENSFILE, std::move(lensFileVal), 1);

        addFloat(*this, result, PBRT_CAMPROP_APERTUREDIAMETER, 1.);
        addFloat(*this, result, PBRT_CAMPROP_FOCALDISTANCE, 10.);

        std::unique_ptr<bool[]> simpleWeightingVal(new bool[1]);
        simpleWeightingVal.get()[0] =
            getPropertyOrValue<bool>(PBRT_CAMPROP_SIMPLEWEIGHTING, true);
        result.AddBool(PBRT_CAMPROP_SIMPLEWEIGHTING,
                       std::move(simpleWeightingVal), 1);
    }

    return result;
}

pbrt::ParamSet PBRTCamera::_filterParamsToPbrtParams()
{
    pbrt::ParamSet result;

    if (!hasProperty("filter_type"))
        return result;

    const auto filterType = getProperty<std::string>(PBRT_FILTERPROP_TYPE);

    if (filterType == PBRT_FILTERTYPE_BOX)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 0.5);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 0.5);
    }
    else if (filterType == PBRT_FILTERTYPE_GAUSSIAN)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_ALPHA, 2.);
    }
    else if (filterType == PBRT_FILTERTYPE_MITCHELL)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 2.);
        const auto val = 1. / 3.;
        addFloat(*this, result, PBRT_FILTERPROP_B, val);
        addFloat(*this, result, PBRT_FILTERPROP_C, val);
    }
    else if (filterType == PBRT_FILTERTYPE_SINC)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 4.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 4.);
        addFloat(*this, result, PBRT_FILTERPROP_TAU, 3.);
    }
    else if (filterType == PBRT_FILTERTYPE_TRIANGLE)
    {
        addFloat(*this, result, PBRT_FILTERPROP_XWIDTH, 2.);
        addFloat(*this, result, PBRT_FILTERPROP_YWIDTH, 2.);
    }

    return result;
}

pbrt::ParamSet PBRTCamera::_filmParamsToPbrtParams()
{
    pbrt::ParamSet result;
    addInt(*this, result, PBRT_FILMPROP_XRESOLUTION,
           static_cast<int>(_currentRes.x));
    addInt(*this, result, PBRT_FILMPROP_YRESOLUTION,
           static_cast<int>(_currentRes.y));
    addFloat(*this, result, PBRT_FILMPROP_SCALE, 1.);
    addFloat(*this, result, PBRT_FILMPROP_DIAGONAL, 35.);
    addFloat(*this, result, PBRT_FILMPROP_MAXSAMPLELUMINANCE, 99999999.9);

    return result;
}

} // namespace brayns
