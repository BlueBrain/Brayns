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
#include <optional>

#include <brayns/core/utils/Logger.h>

#include "Camera.h"
#include "Framebuffer.h"
#include "GeometricModel.h"
#include "Geometry.h"
#include "ImageOperation.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "Render.h"
#include "Renderer.h"
#include "Texture.h"
#include "TransferFunction.h"
#include "Volume.h"
#include "VolumetricModel.h"
#include "World.h"

namespace brayns::experimental
{
class Device
{
private:
    template<typename T>
    using SettingsOf = typename ObjectReflector<T>::Settings;

    template<typename T>
    static constexpr auto nameOf = ObjectReflector<T>::name.c_str();

    template<typename T>
    static void loadParams(auto handle, const auto &settings)
    {
        throwIfNull(handle);
        ObjectReflector<T>::loadParams(handle, settings);
    }

public:
    explicit Device(OSPDevice handle);

    OSPDevice getHandle() const;
    GeometricModel createGeometricModel(const GeometricModelSettings &settings);
    VolumetricModel createVolumetricModel(const VolumetricModelSettings &settings);
    Group createGroup(const GroupSettings &settings);
    Instance createInstance(const InstanceSettings &settings);
    World createWorld(const WorldSettings &settings);
    Framebuffer createFramebuffer(const FramebufferSettings &settings);
    RenderTask render(const RenderSettings &settings);
    std::optional<PickResult> pick(const PickSettings &settings);

    template<std::derived_from<Camera> CameraType>
    CameraType createCamera(const SettingsOf<CameraType> &settings)
    {
        auto handle = ospNewCamera(nameOf<CameraType>);
        loadParams(handle, settings);
        return CameraType(handle);
    }

    template<std::derived_from<Geometry> GeometryType>
    GeometryType createGeometry(const SettingsOf<GeometryType> &settings)
    {
        auto handle = ospNewGeometry(nameOf<GeometryType>);
        loadParams(handle, settings);
        return GeometryType(handle);
    }

    template<std::derived_from<ImageOperation> ImageOperationType>
    ImageOperationType createImageOperation(const SettingsOf<ImageOperationType> &settings)
    {
        auto handle = ospNewImageOperation(nameOf<ImageOperationType>);
        loadParams(handle, settings);
        return ImageOperationType(handle);
    }

    template<std::derived_from<Light> LightType>
    LightType createLight(const SettingsOf<LightType> &settings)
    {
        auto handle = ospNewLight(nameOf<LightType>);
        loadParams(handle, settings);
        return LightType(handle);
    }

    template<std::derived_from<Material> MaterialType>
    MaterialType createMaterial(const SettingsOf<MaterialType> &settings)
    {
        auto handle = ospNewMaterial(nameOf<MaterialType>);
        loadParams(handle, settings);
        return MaterialType(handle);
    }

    template<std::derived_from<Renderer> RendererType>
    RendererType createRenderer(const SettingsOf<RendererType> &settings)
    {
        auto handle = ospNewRenderer(nameOf<RendererType>);
        loadParams(handle, settings);
        return RendererType(handle);
    }

    template<std::derived_from<Texture> TextureType>
    TextureType createTexture(const SettingsOf<TextureType> &settings)
    {
        auto handle = ospNewTexture(nameOf<TextureType>);
        loadParams(handle, settings);
        return TextureType(handle);
    }

    template<std::derived_from<TransferFunction> TransferFunctionType>
    TransferFunctionType createTransferFunction(const SettingsOf<TransferFunctionType> &settings)
    {
        auto handle = ospNewTransferFunction(nameOf<TransferFunctionType>);
        loadParams(handle, settings);
        return TransferFunctionType(handle);
    }

    template<std::derived_from<Volume> VolumeType>
    VolumeType createVolume(const SettingsOf<VolumeType> &settings)
    {
        auto handle = ospNewVolume(nameOf<VolumeType>);
        loadParams(handle, settings);
        return VolumeType(handle);
    }

private:
    struct Deleter
    {
        void operator()(OSPDevice device) const;
    };

    std::unique_ptr<osp::Device, Deleter> _handle;

    void throwIfNull(OSPObject handle) const;
};

Device createDevice(Logger &logger);
}
