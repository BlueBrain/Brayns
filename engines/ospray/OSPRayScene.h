/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/engine/Scene.h>

#include "ispc/model/OSPRayISPCModel.h"

#include <ospray.h>

namespace brayns
{
/**

   OSPRay specific scene

   This object is the OSPRay specific implementation of a scene

*/
class OSPRayScene : public Scene
{
public:
    OSPRayScene(AnimationParameters& animationParameters,
                VolumeParameters& volumeParameters);
    ~OSPRayScene();

    /** @copydoc Scene::commit */
    void commit() final;

    /** @copydoc Scene::commitLights */
    bool commitLights() final;

    /** @copydoc Scene::supportsConcurrentSceneUpdates. */
    bool supportsConcurrentSceneUpdates() const final { return true; }
    ModelPtr createModel() const final;

    OSPModel getModel() { return _rootModel; }
    OSPData lightData() { return _ospLightData; }
    OSPData getSimulationData() { return _ospSimulationData; }
    OSPTransferFunction getTransferFunctionImpl()
    {
        return _ospTransferFunction;
    }

private:
    bool _commitVolumes(std::vector<ModelDescriptorPtr>& modelDescriptors);
    void _commitTransferFunction();
    void _commitSimulationData(
        std::vector<ModelDescriptorPtr>& modelDescriptors);
    void _destroyLights();

    OSPModel _rootModel{nullptr};

    std::vector<float> _simData;
    uint32_t _lastFrame{std::numeric_limits<uint32_t>::max()};
    OSPData _ospSimulationData{nullptr};
    OSPTransferFunction _ospTransferFunction{nullptr};

    std::vector<OSPLight> _ospLights;

    OSPData _ospLightData{nullptr};

    size_t _memoryManagementFlags{0};

    std::vector<ModelDescriptorPtr> _activeModels;
};
} // namespace brayns
