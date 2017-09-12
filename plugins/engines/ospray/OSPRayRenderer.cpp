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

#include <brayns/common/log.h>

#include "OSPRayCamera.h"
#include "OSPRayFrameBuffer.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"

#ifndef __APPLE__
// GCC automtically removes the library if the application does not
// make an explicit use of one of its classes. In the case of OSPRay
// classes are loaded dynamicaly. The following line is only to make
// sure that the hbpKernel library is loaded.
#include <plugins/engines/ospray/ispc/render/ExtendedOBJRenderer.h>
#include <plugins/engines/ospray/ispc/render/ProximityRenderer.h>
brayns::ExtendedOBJRenderer extendedObjRenderer;
brayns::ProximityRenderer proximityRenderer;
#endif

namespace brayns
{
OSPRayRenderer::OSPRayRenderer(const std::string& name,
                               ParametersManager& parametersManager)
    : Renderer(parametersManager)
    , _name(name)
    , _camera(0)
{
    _renderer = ospNewRenderer(name.c_str());
    assert(_renderer);
}

OSPRayRenderer::~OSPRayRenderer()
{
    ospRelease(_renderer);
}

void OSPRayRenderer::render(FrameBufferPtr frameBuffer)
{
    OSPRayFrameBuffer* osprayFrameBuffer =
        dynamic_cast<OSPRayFrameBuffer*>(frameBuffer.get());
    ospRenderFrame(osprayFrameBuffer->impl(), _renderer,
                   OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM);
}

void OSPRayRenderer::commit()
{
    RenderingParameters& rp = _parametersManager.getRenderingParameters();
    SceneParameters& sp = _parametersManager.getSceneParameters();
    VolumeParameters& vp = _parametersManager.getVolumeParameters();
    ShadingType mt = rp.getShading();

    Vector3f color = rp.getBackgroundColor();
    ospSet3f(_renderer, "bgColor", color.x(), color.y(), color.z());
    ospSet1f(_renderer, "shadows", rp.getShadows());
    ospSet1f(_renderer, "softShadows", rp.getSoftShadows());
    ospSet1f(_renderer, "ambientOcclusionStrength",
             rp.getAmbientOcclusionStrength());

    ospSet1i(_renderer, "shadingEnabled", (mt == ShadingType::diffuse));
    ospSet1f(_renderer, "timestamp", sp.getTimestamp());
    ospSet1i(_renderer, "randomNumber", rand() % 10000);
    ospSet1i(_renderer, "spp", rp.getSamplesPerPixel());
    ospSet1i(_renderer, "electronShading", (mt == ShadingType::electron));
    ospSet1f(_renderer, "epsilon", rp.getEpsilon());
    ospSet1i(_renderer, "moving", false);
    ospSet1f(_renderer, "detectionDistance", rp.getDetectionDistance());
    ospSet1i(_renderer, "detectionOnDifferentMaterial",
             rp.getDetectionOnDifferentMaterial());
    color = rp.getDetectionNearColor();
    ospSet3f(_renderer, "detectionNearColor", color.x(), color.y(), color.z());
    color = rp.getDetectionFarColor();
    ospSet3f(_renderer, "detectionFarColor", color.x(), color.y(), color.z());
    ospSet1i(_renderer, "materialForSimulation", MATERIAL_SIMULATION);
    ospSet1i(_renderer, "volumeSamplesPerRay", vp.getSamplesPerRay());

    OSPRayScene* osprayScene = static_cast<OSPRayScene*>(_scene.get());
    assert(osprayScene);

    const float ts =
        _scene->getParametersManager().getSceneParameters().getTimestamp();
    const auto model = osprayScene->modelImpl(ts);
    if (model)
        ospSetObject(_renderer, "world", *model);
    else
        BRAYNS_ERROR << "No model found for timestamp " << ts << std::endl;

    const auto simulationModel = osprayScene->simulationModelImpl();
    if (simulationModel)
        ospSetObject(_renderer, "simulationModel", *simulationModel);
    ospCommit(_renderer);
}

void OSPRayRenderer::setCamera(CameraPtr camera)
{
    _camera = static_cast<OSPRayCamera*>(camera.get());
    assert(_camera);
    ospSetObject(_renderer, "camera", _camera->impl());
    ospCommit(_renderer);
}
}
