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
#ifndef PBRTSCENE_H
#define PBRTSCENE_H

#include <brayns/engine/Scene.h>

#include <pbrt/core/scene.h>

namespace brayns
{
class PBRTScene : public Scene
{
public:
    PBRTScene(AnimationParameters& animationParameters,
              GeometryParameters& geometryParameters,
              VolumeParameters& volumeParameters);
    ~PBRTScene();

    void commit() final;

    bool commitLights() final;

    bool supportsConcurrentSceneUpdates() const final { return false; }

    ModelPtr createModel() const final;

    void setCurrentRenderer(const std::string& renderer) { _currentRenderer = renderer; }

    const pbrt::Scene* getPBRTScene() const { return _pbrtScene.get(); }

    bool needsToRender() const { return _needsRender; }
    void setNeedsToRender(const bool v) { _needsRender = v; }

private:
    bool _needsRender{true};
    std::unique_ptr<pbrt::Scene> _pbrtScene {nullptr};
    std::vector<std::shared_ptr<pbrt::Light>> _lights;
    std::vector<std::shared_ptr<pbrt::GeometricPrimitive>> _lightShapes;

    //TransformPool _transformPool;
    std::vector<std::unique_ptr<pbrt::Transform>> _transformPool;

    std::string _currentRenderer;
};
}

#endif
