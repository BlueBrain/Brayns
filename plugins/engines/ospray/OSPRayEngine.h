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

#ifndef OSPRAYENGINE_H
#define OSPRAYENGINE_H

#include <brayns/common/engine/Engine.h>

namespace brayns
{
/**
 * OSPRay implementation of the ray-tracing engine.
 */
class OSPRayEngine : public Engine
{
public:
    OSPRayEngine(int argc, const char** argv,
                 ParametersManager& parametersManager);

    ~OSPRayEngine();

    /** @copydoc Engine::name */
    EngineType name() const final;

    /** @copydoc Engine::commit */
    void commit() final;

    /** @copydoc Engine::render */
    void render() final;

    /** @copydoc Engine::preRender */
    void preRender() final;

    /** @copydoc Engine::postRender */
    void postRender() final;

    /**
     * Constrain size to multiples of the OSPRay tile size in case of streaming
     * using the DeflectPixelOp.
     */
    Vector2ui getSupportedFrameSize(const Vector2ui& size) final;

    /** @copydoc Engine::getMinimumFrameSize */
    Vector2ui getMinimumFrameSize() const final;

    /** @copydoc Engine::haveDeflectPixelOp */
    bool haveDeflectPixelOp() const final { return _haveDeflectPixelOp; }
private:
    bool _haveDeflectPixelOp{false};
    bool _useDynamicLoadBalancer{false};
};
}

#endif // OSPRAYENGINE_H
