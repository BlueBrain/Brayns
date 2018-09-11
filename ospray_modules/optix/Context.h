/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include <ospray/SDK/common/Data.h>

#include <optixu/optixpp_namespace.h>

#include <memory>
#include <mutex>
#include <unordered_map>

#include "geom/Geometry.h"

namespace ospray
{
struct Texture2D;
}

namespace bbp
{
namespace optix
{
class Context
{
public:
    ~Context();
    static Context& get();
    static void destroy();

    ::optix::Context getOptixContext();
    ::optix::Material createMaterial(bool textured);
    void createTexture(ospray::Texture2D* tx);
    void deleteTexture(ospray::Texture2D* tx);
    ::optix::TextureSampler getTextureSampler(ospray::Texture2D* tx);
    void updateLights(
        ospray::Ref<ospray::Data> lightData = ospray::Ref<ospray::Data>());

    ::optix::Geometry createGeometry(Geometry::Type type);

    std::unique_lock<std::mutex> getScopeLock()
    {
        return std::unique_lock<std::mutex>(_mutex);
    }

private:
    Context();

    void _initialize();
    static std::unique_ptr<Context> _context;

    ::optix::Context _optixContext;
    ::optix::Program _phong_ah;
    ::optix::Program _phong_ch;
    ::optix::Program _phong_ch_textured;
    std::array<::optix::Program, Geometry::Type::SIZE> _bounds;
    std::array<::optix::Program, Geometry::Type::SIZE> _intersects;

    ospray::Ref<ospray::Data> _lightData;
    ::optix::Buffer _lightBuffer;

    std::unordered_map<void*, ::optix::TextureSampler> _optixTextureSamplers;
    std::mutex _mutex;
};
}
}
