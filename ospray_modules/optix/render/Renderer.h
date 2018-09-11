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

#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/render/Renderer.h>

#include <vector>

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

#include "../CommonStructs.h"
#include "../framebuffer/FrameBuffer.h"

namespace bbp
{
namespace optix
{
struct Renderer : public ospray::Renderer
{
    Renderer();
    ~Renderer();
    std::string toString() const override;
    void commit() override;

    float renderFrame(ospray::FrameBuffer* fb,
                      const ospray::uint32 fbChannelFlags) override;

    void* beginFrame(ospray::FrameBuffer* fb) override;

    void endFrame(void* perFrameData,
                  const ospray::int32 fbChannelFlags) override;

    void renderTile(void* perFrameData, ospray::Tile& tile,
                    size_t jobID) const override;

private:
    void _updateVolume();
    void _updateTransferFunction();
    float _mpiRenderFrame(ospray::FrameBuffer* fb,
                          const ospray::uint32 fbChannelFlags);

    ::optix::Context _context;

    ::optix::Buffer _colorMapBuffer{nullptr};
    ::optix::Buffer _emissionIntensityMapBuffer{nullptr};

    ospray::Ref<ospray::Data> _transferFunctionDiffuseData;
    ospray::Ref<ospray::Data> _transferFunctionEmissionData;

    FrameBuffer* _frameBuffer{nullptr}; // MPI slave only

    ::optix::Buffer _volumeBuffer{nullptr};
};

// Inlined member functions ///////////////////////////////////////////////

inline std::string Renderer::toString() const
{
    return "ospray::Renderer";
}
}
}
