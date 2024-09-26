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

#include <any>
#include <concepts>
#include <functional>

#include <brayns/core/engine/ImageOperation.h>
#include <brayns/core/manager/ObjectManager.h>

namespace brayns
{
struct UserImageOperation
{
    std::any value;
    std::function<ImageOperation()> get;
};

template<ReflectedJson Settings, std::derived_from<ImageOperation> T>
struct UserImageOperationOf
{
    Settings settings;
    T value;
};

template<>
struct JsonObjectReflector<ToneMapperSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ToneMapperSettings>();
        builder.field("exposure", [](auto &object) { return &object.exposure; }).description("Amount of light per unit area").defaultValue(1.0F);
        builder.field("contrast", [](auto &object) { return &object.contrast; }).description("Constrast (toe of the curve)").defaultValue(1.6773F);
        builder.field("shoulder", [](auto &object) { return &object.shoulder; })
            .description("Highlight compression (shoulder of the curve)")
            .defaultValue(0.9714F);
        builder.field("midIn", [](auto &object) { return &object.midIn; }).description("Mid-level anchor input").defaultValue(0.18F);
        builder.field("midOut", [](auto &object) { return &object.midOut; }).description("Mid-level anchor output").defaultValue(0.18F);
        builder.field("hdrMax", [](auto &object) { return &object.hdrMax; })
            .description("Maximum HDR input that is not clipped")
            .defaultValue(11.0785F);
        builder.field("acesColor", [](auto &object) { return &object.acesColor; }).description("Apply the ACES color transforms").defaultValue(true);
        return builder.build();
    }
};

using CreateToneMapperParams = CreateParamsOf<ToneMapperSettings>;
using GetToneMapperResult = GetResultOf<ToneMapperSettings>;
using UpdateToneMapperParams = UpdateParamsOf<ToneMapperSettings>;
using UserToneMapper = UserImageOperationOf<ToneMapperSettings, ToneMapper>;

CreateObjectResult createToneMapper(ObjectManager &objects, Device &device, const CreateToneMapperParams &params);
GetToneMapperResult getToneMapper(ObjectManager &objects, const GetObjectParams &params);
void updateToneMapper(ObjectManager &objects, Device &device, const UpdateToneMapperParams &params);
}
