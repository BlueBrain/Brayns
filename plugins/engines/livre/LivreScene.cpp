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

#include "LivreScene.h"

#include <brayns/parameters/ParametersManager.h>

#include <livre/data/DataSource.h>
#include <livre/eq/Engine.h>
#include <livre/eq/FrameData.h>
#include <livre/eq/settings/RenderSettings.h>
#include <livre/lib/configuration/VolumeRendererParameters.h>

namespace brayns
{
LivreScene::LivreScene(const Renderers& renderers,
                       ParametersManager& parametersManager,
                       livre::Engine& livre)
    : Scene(renderers, parametersManager)
    , _livre(livre)
{
    auto& braynsParams = _parametersManager.getVolumeParameters();
    const auto& livreParams = _livre.getFrameData().getVRParameters();
    braynsParams.setSamplesPerRay(livreParams.getSamplesPerRay());
}

void LivreScene::commitTransferFunctionData()
{
    auto& livreTF =
        _livre.getFrameData().getRenderSettings().getTransferFunction();
    const auto& diffuseColors = getTransferFunction().getDiffuseColors();

    livreTF.getDiffuse().resize(diffuseColors.size());
    livreTF.getAlpha().resize(diffuseColors.size());
    for (size_t i = 0; i < diffuseColors.size(); ++i)
    {
        const auto& color = diffuseColors[i];
        livreTF.getDiffuse()[i] = {color[0], color[1], color[2]};
        livreTF.getAlpha()[i] = color[3];
    }
    const auto& range = getTransferFunction().getValuesRange();
    livreTF.setRange({range[0], range[1]});
}

void LivreScene::commitVolumeData()
{
    const auto& braynsParams = _parametersManager.getVolumeParameters();
    auto& livreParams = _livre.getFrameData().getVRParameters();
    livreParams.setSamplesPerRay(braynsParams.getSamplesPerRay());
}

void LivreScene::commit()
{
}

void LivreScene::buildGeometry()
{
    const auto& volInfo = _livre.getVolumeInformation();
    Vector4f halfWorldSize = volInfo.worldSize / 2.f;
    halfWorldSize[3] = 1.f;
    auto bboxMin = volInfo.dataToLivreTransform.inverse() * halfWorldSize;
    bboxMin[3] = 1;
    auto bboxMax = volInfo.dataToLivreTransform.inverse() * -halfWorldSize;
    bboxMax[3] = 1;

    Boxf& worldBounds = getWorldBounds();
    worldBounds.reset();
    worldBounds.merge(Vector3f(bboxMin) / volInfo.meterToDataUnitRatio);
    worldBounds.merge(Vector3f(bboxMax) / volInfo.meterToDataUnitRatio);
}

uint64_t LivreScene::serializeGeometry()
{
    return 0;
}

void LivreScene::commitLights()
{
}

void LivreScene::commitMaterials(const bool /*updateOnly*/)
{
}

void LivreScene::commitSimulationData()
{
}

void LivreScene::loadSceneFromCacheFile()
{
}

void LivreScene::saveSceneToCacheFile()
{
}

bool LivreScene::isVolumeSupported(const std::string& volumeFile) const
{
    return livre::DataSource::handles(servus::URI(volumeFile));
}
}
