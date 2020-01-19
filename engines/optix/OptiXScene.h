/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/types.h>
#include <brayns/engineapi/Scene.h>

#include <optixu/optixpp_namespace.h>

#include "CommonStructs.h"

namespace brayns
{
/**

   OptiX specific scene

   This object is the OptiX specific implementation of a scene

*/
class OptiXScene : public Scene
{
public:
    OptiXScene(AnimationParameters& animationParameters,
               GeometryParameters& geometryParameters,
               VolumeParameters& volumeParameters);
    ~OptiXScene();

    /** @copydoc Scene::commit */
    void commit() final;

    /** @copydoc Scene::commitLights */
    bool commitLights() final;

    /** @copydoc Scene::createModel */
    ModelPtr createModel() const final;

    /** @copydoc Scene::supportsConcurrentSceneUpdates. */
    bool supportsConcurrentSceneUpdates() const final { return false; }

private:
    optix::Buffer _lightBuffer{nullptr};
    std::vector<BasicLight> _optixLights;
    ::optix::Group _rootGroup{nullptr};

    // Material Lookup tables
    optix::Buffer _colorMapBuffer{nullptr};
    optix::Buffer _emissionIntensityMapBuffer{nullptr};
    ::optix::TextureSampler _backgroundTextureSampler{nullptr};
    ::optix::TextureSampler _dummyTextureSampler{nullptr};

    // Volumes
    optix::Buffer _volumeBuffer;
};
} // namespace brayns
