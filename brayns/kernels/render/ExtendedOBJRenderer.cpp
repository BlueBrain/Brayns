/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

// obj
#include "ExtendedOBJRenderer.h"
#include "ExtendedOBJMaterial.h"
// ospray
#include <ospray/common/Model.h>
#include <ospray/common/Data.h>
#include <ospray/camera/Camera.h>
#include <ospray/lights/Light.h>
//embree
#include <embree2/rtcore.h>
//sys
#include <vector>
// ispc exports
#include "ExtendedOBJRenderer_ispc.h"

using namespace ospray;

namespace brayns
{

void ExtendedOBJRenderer::commit()
{
    Renderer::commit();

    lightData = (ospray::Data*)getParamData("lights");

    lightArray.clear();

    if (lightData)
        for (size_t i=0; i < lightData->size(); ++i)
            lightArray.push_back(((Light**)lightData->data)[i]->getIE());

    void **lightPtr = lightArray.empty() ? NULL : &lightArray[0];

    vec3f bgColor;
    bgColor = getParam3f("bgColor", vec3f(1.f));

    shadowsEnabled =
            bool(getParam1i("shadowsEnabled", 1));
    softShadowsEnabled =
            bool(getParam1i("softShadowsEnabled", 1));
    ambientOcclusionEnabled =
            bool(getParam1i("ambientOcclusionEnabled", 1));
    shadingEnabled =
            bool(getParam1i("shadingEnabled", 1));
    randomNumber =
            getParam1i("randomNumber", 0);
    moving =
            bool(getParam1i("moving", 0));
    frameNumber =
            getParam1i("frameNumber",0);
    spp =
            getParam1i("spp", 1);
    electronShadingEnabled =
            bool(getParam1i("electronShading", 0));
    lightEmittingMaterialsEnabled =
            bool(getParam1i("lightEmittingMaterialsEnabled", 0));
    gradientBackgroundEnabled =
            bool(getParam1i("gradientBackgroundEnabled", 0));

    ispc::ExtendedOBJRenderer_set(
                getIE(),
                (ispc::vec3f&)bgColor,
                shadowsEnabled,
                softShadowsEnabled,
                ambientOcclusionEnabled,
                shadingEnabled,
                randomNumber,
                moving,
                frameNumber,
                spp,
                electronShadingEnabled,
                lightEmittingMaterialsEnabled,
                gradientBackgroundEnabled,
                lightPtr, lightArray.size());
}

ExtendedOBJRenderer::ExtendedOBJRenderer()
{
    ispcEquivalent = ispc::ExtendedOBJRenderer_create(this);
}

/*! \brief create a material of given type */
Material *ExtendedOBJRenderer::createMaterial(const char *)
{
    Material *mat = new brayns::obj::ExtendedOBJMaterial;
    return mat;
}

OSP_REGISTER_RENDERER(ExtendedOBJRenderer,EXOBJ);
OSP_REGISTER_RENDERER(ExtendedOBJRenderer,exobj);
} // ::brayns
