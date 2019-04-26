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

#include <optixu/optixpp_namespace.h>

#include <memory>
#include <mutex>
#include <unordered_map>

namespace brayns
{
enum class OptixGeometryType
{
    sphere,
    cone,
    cylinder,
    triangleMesh
};

struct OptixShaderProgram
{
    ::optix::Program any_hit{nullptr};
    ::optix::Program closest_hit{nullptr};
    ::optix::Program closest_hit_textured{nullptr};
};

class OptiXContext
{
public:
    ~OptiXContext();
    static OptiXContext& get();

    ::optix::Context getOptixContext() { return _optixContext; }
    // Camera
    ::optix::Program createCamera();

    // Geometry
    ::optix::Geometry createGeometry(const OptixGeometryType type);
    ::optix::GeometryGroup createGeometryGroup(const bool compact);
    ::optix::Group createGroup();
    ::optix::Material createMaterial();

    // Textures
    ::optix::TextureSampler createTextureSampler(Texture2DPtr texture);

    // Others
    void addRenderer(const std::string& name,
                     const OptixShaderProgram& program);
    const OptixShaderProgram& getRenderer(const std::string& name);

    std::unique_lock<std::mutex> getScopeLock()
    {
        return std::unique_lock<std::mutex>(_mutex);
    }

private:
    OptiXContext();

    void _initialize();
    void _printSystemInformation() const;

    static std::unique_ptr<OptiXContext> _context;

    ::optix::Context _optixContext{nullptr};

    std::map<std::string, OptixShaderProgram> _rendererProgram;

    std::map<OptixGeometryType, ::optix::Program> _bounds;
    std::map<OptixGeometryType, ::optix::Program> _intersects;

    std::unordered_map<void*, ::optix::TextureSampler> _optixTextureSamplers;
    std::mutex _mutex;
};
} // namespace brayns
