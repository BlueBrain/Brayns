/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "LivreCamera.h"

#include <livre/core/data/VolumeInformation.h>
#include <livre/eq/Engine.h>
#include <livre/eq/FrameData.h>
#include <livre/eq/settings/CameraSettings.h>
#include <livre/eq/settings/RenderSettings.h>

#include <brayns/common/log.h>

namespace brayns
{
LivreCamera::LivreCamera(const CameraType cameraType, livre::Engine& livre)
    : Camera(cameraType)
    , _livre(livre)
{
}

void LivreCamera::commit()
{
    const Matrix4f braynsModelView(getPosition(), getTarget(), getUp());

    Vector4f translation = braynsModelView.getColumn(3);
    Matrix4f rotation;
    rotation.setSubMatrix<3, 3>(braynsModelView.getSubMatrix<3, 3>(0, 0), 0, 0);

    const auto& volumeInformation = _livre.getVolumeInformation();
    translation = -rotation.inverse() * translation;
    translation *= volumeInformation.meterToDataUnitRatio;
    translation[3] = 1.0f;

    translation =
        -rotation * volumeInformation.dataToLivreTransform * translation;
    translation[3] = 1.0f;

    Matrix4f livreModelView;
    livreModelView.setSubMatrix<3, 3>(rotation.getSubMatrix<3, 3>(0, 0), 0, 0);
    livreModelView.setColumn(3, translation);

    _livre.getFrameData().getCameraSettings().setModelViewMatrix(
        livreModelView);

    std::vector<::lexis::render::detail::Plane> planes;
    for (const auto& clipPlane : getClipPlanes())
    {
        ::lexis::render::detail::Plane plane;
        float normal[3] = {clipPlane.x(), clipPlane.y(), clipPlane.z()};
        plane.setNormal(normal);
        plane.setD(clipPlane.w());
        planes.push_back(plane);
    }
    ::livre::ClipPlanes clipPlanes;
    clipPlanes.setPlanes(planes);
    _livre.getFrameData().getRenderSettings().setClipPlanes(clipPlanes);
}

void LivreCamera::setEnvironmentMap(const bool /*environmentMap*/)
{
    BRAYNS_WARN << "Environment map is not supported by Livre engine"
                << std::endl;
}
}
