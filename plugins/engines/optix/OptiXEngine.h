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

#ifndef OPTIXENGINE_H
#define OPTIXENGINE_H

#include <brayns/common/engine/Engine.h>

// OptiX
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

namespace brayns
{
/**
 * OptiX implementation of the ray-tracing engine.
 */
class OptiXEngine : public Engine
{
public:
    OptiXEngine(int argc, const char** argv,
                ParametersManager& parametersManager);

    ~OptiXEngine();

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

    /** @copydoc Engine::getMinimumFrameSize */
    Vector2ui getMinimumFrameSize() const final;

    uint64_t getTotalMemory() { return _totalMemory; }
private:
    void _initializeContext();

    optix::Context _context;
    uint64_t _totalMemory;
    std::string _workingDirectory;
};
}

#endif // OPTIXENGINE_H
