/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <concepts>
#include <memory>

#include <ospray/ospray_cpp.h>

#include "Camera.h"
#include "Framebuffer.h"
#include "Geometry.h"
#include "Light.h"
#include "Material.h"
#include "Render.h"
#include "Renderer.h"
#include "Volume.h"
#include "World.h"

namespace brayns::experimental
{
class Device
{
public:
    explicit Device(OSPDevice handle);

    GeometricModel createGeometricModel();
    VolumetricModel createVolumetricModel();
    Group createGroup();
    Instance createInstance();
    World createWorld();
    Framebuffer createFramebuffer(const FramebufferSettings &settings);
    RenderTask render(const RenderSettings &settings);

    template<std::derived_from<Camera> CameraType>
    CameraType createCamera()
    {
        const auto &name = CameraType::name;
        auto handle = ospNewCamera(name.c_str());
        return CameraType(handle);
    }

    template<std::derived_from<ImageOperation> ImageOperationType>
    ImageOperationType createImageOperation()
    {
        const auto &name = ImageOperationType::name;
        auto handle = ospNewImageOperation(name.c_str());
        return ImageOperationType(handle);
    }

    template<std::derived_from<Geometry> GeometryType>
    GeometryType createGeometry()
    {
        const auto &name = GeometryType::name;
        auto handle = ospNewGeometry(name.c_str());
        return GeometryType(handle);
    }

    template<std::derived_from<Light> LightType>
    LightType createLight()
    {
        const auto &name = LightType::name;
        auto handle = ospNewLight(name.c_str());
        return LightType(handle);
    }

    template<std::derived_from<Material> MaterialType>
    MaterialType createMaterial()
    {
        const auto &name = MaterialType::name;
        auto handle = ospNewMaterial(name.c_str());
        return MaterialType(handle);
    }

    template<std::derived_from<Renderer> RendererType>
    RendererType createRenderer()
    {
        const auto &name = RendererType::name;
        auto handle = ospNewRenderer(name.c_str());
        return RendererType(handle);
    }

    template<std::derived_from<TransferFunction> TransferFunctionType>
    TransferFunctionType createTransferFunction()
    {
        const auto &name = TransferFunctionType::name;
        auto handle = ospNewTransferFunction(name.c_str());
        return TransferFunctionType(handle);
    }

    template<std::derived_from<Volume> VolumeType>
    VolumeType createVolume()
    {
        const auto &name = VolumeType::name;
        auto handle = ospNewVolume(name.c_str());
        return VolumeType(handle);
    }

private:
    struct Deleter
    {
        void operator()(OSPDevice device) const;
    };

    std::unique_ptr<osp::Device, Deleter> _handle;
};
}
