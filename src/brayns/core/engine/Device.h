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

namespace brayns
{
class Device
{
public:
    explicit Device(OSPDevice handle);

    OSPDevice getHandle() const;
    Future render(const Context &context);
    std::optional<PickResult> pick(const PickSettings &settings);

    template<ReflectedObjectSettings T>
    T create(const SettingsOf<T> &settings)
    {
        auto handle = ObjectReflector<T>::createHandle(_handle.get(), settings);
        return T(handle);
    }

private:
    struct Deleter
    {
        void operator()(OSPDevice device) const;
    };

    std::unique_ptr<osp::Device, Deleter> _handle;
};

Device createDevice(Logger &logger);
}
