/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef OSPRAYSCENE_H
#define OSPRAYSCENE_H

#include <brayns/common/types.h>
#include <brayns/engineapi/Scene.h>

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
                GeometryParameters& geometryParameters,
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
    ModelDescriptorPtr getSimulatedModel();

private:
    bool _commitVolumeAndTransferFunction(ModelDescriptors& modelDescriptors);
    void _destroyLights();

    OSPModel _rootModel{nullptr};

    std::vector<OSPLight> _ospLights;

    OSPData _ospLightData{nullptr};

    size_t _memoryManagementFlags{0};

    ModelDescriptors _activeModels;
};
} // namespace brayns
#endif // OSPRAYSCENE_H
