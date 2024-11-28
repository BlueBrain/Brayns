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

#include <brayns/core/engine/Light.h>
#include <brayns/core/manager/ObjectManager.h>

#include "TextureObjects.h"

namespace brayns
{
struct UserLight
{
    LightSettings settings;
    std::any value;
    std::function<Light()> get;
};

template<>
struct JsonObjectReflector<LightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<LightSettings>();
        builder.field("color", [](auto &object) { return &object.color; }).description("Light color RGB").defaultValue(Color3(1.0F, 1.0F, 1.0F));
        builder.field("visible", [](auto &object) { return &object.visible; })
            .description("Whether the light can be directly seen")
            .defaultValue(true);
        return builder.build();
    }
};

using GetLightResult = GetResultOf<LightSettings>;
using UpdateLightParams = UpdateParamsOf<LightSettings>;

GetLightResult getLight(ObjectManager &objects, const GetObjectParams &params);
void updateLight(ObjectManager &objects, Device &device, const UpdateLightParams &params);

template<typename Storage, std::derived_from<Light> T>
struct UserLightOf
{
    Storage storage;
    T value;
};

template<ReflectedJsonObject T>
using CreateLightParamsOf = CreateParamsOf<ComposedParamsOf<LightSettings, T>>;

template<>
struct JsonObjectReflector<Power>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Power>();
        builder.constant("type", "Power");
        builder.field("value", [](auto &object) { return &object.value; }).description("Overall power in W").defaultValue(1.0F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Intensity>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Intensity>();
        builder.constant("type", "Intensity");
        builder.field("value", [](auto &object) { return &object.value; }).description("Directional power in W/sr").defaultValue(1.0F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Radiance>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Radiance>();
        builder.constant("type", "Radiance");
        builder.field("value", [](auto &object) { return &object.value; }).description("Directional power per surface in W/sr/m2").defaultValue(1.0F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Irradiance>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Irradiance>();
        builder.constant("type", "Irradiance");
        builder.field("value", [](auto &object) { return &object.value; }).description("Power per surface in W/m2").defaultValue(1.0F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<DistantLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<DistantLightSettings>();
        builder.field("direction", [](auto &object) { return &object.direction; })
            .description("Light emission direction XYZ")
            .defaultValue(Vector3{0.0F, 0.0F, -1.0F});
        builder.field("angularDiameter", [](auto &object) { return &object.angularDiameter; })
            .description("Apparent size (angle in radians) of the light")
            .defaultValue(0.0F);
        builder.field("intensity", [](auto &object) { return &object.intensity; }).description("Light intensity").defaultValue(Irradiance{});
        return builder.build();
    }
};

using CreateDistantLightParams = CreateLightParamsOf<DistantLightSettings>;
using GetDistantLightResult = GetResultOf<DistantLightSettings>;
using UpdateDistantLightParams = UpdateParamsOf<DistantLightSettings>;
using UserDistantLight = UserLightOf<DistantLightSettings, DistantLight>;

CreateObjectResult createDistantLight(ObjectManager &objects, Device &device, const CreateDistantLightParams &params);
GetDistantLightResult getDistantLight(ObjectManager &objects, const GetObjectParams &params);
void updateDistantLight(ObjectManager &objects, Device &device, const UpdateDistantLightParams &params);

template<>
struct JsonObjectReflector<SphereLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<SphereLightSettings>();
        builder.field("position", [](auto &object) { return &object.position; }).description("Light position").defaultValue(Vector3{0.0F, 0.0F, 0.0F});
        builder.field("radius", [](auto &object) { return &object.radius; }).description("Sphere radius (0 = point light)").defaultValue(0.0F);
        builder.field("intensity", [](auto &object) { return &object.intensity; }).description("Light intensity").defaultValue(Intensity{});
        return builder.build();
    }
};

using CreateSphereLightParams = CreateLightParamsOf<SphereLightSettings>;
using GetSphereLightResult = GetResultOf<SphereLightSettings>;
using UpdateSphereLightParams = UpdateParamsOf<SphereLightSettings>;
using UserSphereLight = UserLightOf<SphereLightSettings, SphereLight>;

CreateObjectResult createSphereLight(ObjectManager &objects, Device &device, const CreateSphereLightParams &params);
GetSphereLightResult getSphereLight(ObjectManager &objects, const GetObjectParams &params);
void updateSphereLight(ObjectManager &objects, Device &device, const UpdateSphereLightParams &params);

template<>
struct JsonObjectReflector<SpotLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<SpotLightSettings>();
        builder.field("position", [](auto &object) { return &object.position; })
            .description("Light position XYZ")
            .defaultValue(Vector3{0.0F, 0.0F, 0.0F});
        builder.field("direction", [](auto &object) { return &object.direction; })
            .description("Light direction XYZ")
            .defaultValue(Vector3{0.0F, 0.0F, -1.0F});
        builder.field("openingAngle", [](auto &object) { return &object.openingAngle; })
            .description("Opening angle in radians")
            .defaultValue(radians(180.0F));
        builder.field("penumbraAngle", [](auto &object) { return &object.penumbraAngle; })
            .description("Penumbra angle in radians")
            .defaultValue(radians(5.0F));
        builder.field("outerRadius", [](auto &object) { return &object.outerRadius; }).description("Light outer ring radius").defaultValue(0.0F);
        builder.field("innerRadius", [](auto &object) { return &object.innerRadius; }).description("Light inner ring radius").defaultValue(0.0F);
        builder.field("intensity", [](auto &object) { return &object.intensity; }).description("Light intensity").defaultValue(Intensity{});
        return builder.build();
    }
};

using CreateSpotLightParams = CreateLightParamsOf<SpotLightSettings>;
using GetSpotLightResult = GetResultOf<SpotLightSettings>;
using UpdateSpotLightParams = UpdateParamsOf<SpotLightSettings>;
using UserSpotLight = UserLightOf<SpotLightSettings, SpotLight>;

CreateObjectResult createSpotLight(ObjectManager &objects, Device &device, const CreateSpotLightParams &params);
GetSpotLightResult getSpotLight(ObjectManager &objects, const GetObjectParams &params);
void updateSpotLight(ObjectManager &objects, Device &device, const UpdateSpotLightParams &params);

template<>
struct JsonObjectReflector<QuadLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<QuadLightSettings>();
        builder.field("position", [](auto &object) { return &object.position; })
            .description("Light position XYZ")
            .defaultValue(Vector3{0.0F, 0.0F, 0.0F});
        builder.field("edge1", [](auto &object) { return &object.edge1; }).description("First edge XYZ").defaultValue(Vector3{0.0F, 1.0F, 0.0F});
        builder.field("edge2", [](auto &object) { return &object.edge2; }).description("Second edge XYZ").defaultValue(Vector3{1.0F, 0.0F, 0.0F});
        builder.field("intensity", [](auto &object) { return &object.intensity; })
            .description("Light intensity (direction = edge1 x edge2)")
            .defaultValue(Radiance{});
        return builder.build();
    }
};

using CreateQuadLightParams = CreateLightParamsOf<QuadLightSettings>;
using GetQuadLightResult = GetResultOf<QuadLightSettings>;
using UpdateQuadLightParams = UpdateParamsOf<QuadLightSettings>;
using UserQuadLight = UserLightOf<QuadLightSettings, QuadLight>;

CreateObjectResult createQuadLight(ObjectManager &objects, Device &device, const CreateQuadLightParams &params);
GetQuadLightResult getQuadLight(ObjectManager &objects, const GetObjectParams &params);
void updateQuadLight(ObjectManager &objects, Device &device, const UpdateQuadLightParams &params);

template<>
struct JsonObjectReflector<CylinderLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CylinderLightSettings>();
        builder.field("start", [](auto &object) { return &object.start; }).description("Cylinder start XYZ").defaultValue(Vector3{0.0F, 0.0F, 0.0F});
        builder.field("end", [](auto &object) { return &object.end; }).description("Cylinder end XYZ").defaultValue(Vector3{0.0F, 0.0F, 1.0F});
        builder.field("radius", [](auto &object) { return &object.radius; }).description("Cylinder radius").defaultValue(1.0F);
        builder.field("intensity", [](auto &object) { return &object.intensity; }).description("Light intensity").defaultValue(Radiance{});
        return builder.build();
    }
};

using CreateCylinderLightParams = CreateLightParamsOf<CylinderLightSettings>;
using GetCylinderLightResult = GetResultOf<CylinderLightSettings>;
using UpdateCylinderLightParams = UpdateParamsOf<CylinderLightSettings>;
using UserCylinderLight = UserLightOf<CylinderLightSettings, CylinderLight>;

CreateObjectResult createCylinderLight(ObjectManager &objects, Device &device, const CreateCylinderLightParams &params);
GetCylinderLightResult getCylinderLight(ObjectManager &objects, const GetObjectParams &params);
void updateCylinderLight(ObjectManager &objects, Device &device, const UpdateCylinderLightParams &params);

struct HdriLightParams
{
    HdriLightSettings value;
    ObjectId map;
};

template<>
struct JsonObjectReflector<HdriLightParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<HdriLightParams>();
        builder.field("map", [](auto &object) { return &object.map; }).description("ID of the texture 2D to use as light map");
        builder.field("up", [](auto &object) { return &object.value.up; })
            .description("Up direction of the light XYZ")
            .defaultValue(Vector3{0.0F, 1.0F, 0.0F});
        builder.field("direction", [](auto &object) { return &object.value.direction; })
            .description("Direction to map the center of the texture XYZ")
            .defaultValue(Vector3{0.0F, 0.0F, 1.0F});
        builder.field("intensityScale", [](auto &object) { return &object.value.intensityScale; })
            .description("Scale texture light intensity by this factor")
            .defaultValue(1.0F);
        return builder.build();
    }
};

struct HdriLightStorage
{
    HdriLightSettings settings;
    Stored<UserTexture> map;
};

using CreateHdriLightParams = CreateLightParamsOf<HdriLightParams>;
using GetHdriLightResult = GetResultOf<HdriLightParams>;
using UpdateHdriLightParams = UpdateParamsOf<HdriLightParams>;
using UserHdriLight = UserLightOf<HdriLightStorage, HdriLight>;

CreateObjectResult createHdriLight(ObjectManager &objects, Device &device, const CreateHdriLightParams &params);
GetHdriLightResult getHdriLight(ObjectManager &objects, const GetObjectParams &params);
void updateHdriLight(ObjectManager &objects, Device &device, const UpdateHdriLightParams &params);

template<>
struct JsonObjectReflector<AmbientLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<AmbientLightSettings>();
        builder.field("intensity", [](auto &object) { return &object.intensity; }).description("Light intensity").defaultValue(Irradiance{});
        return builder.build();
    }
};

using CreateAmbientLightParams = CreateLightParamsOf<AmbientLightSettings>;
using GetAmbientLightResult = GetResultOf<AmbientLightSettings>;
using UpdateAmbientLightParams = UpdateParamsOf<AmbientLightSettings>;
using UserAmbientLight = UserLightOf<AmbientLightSettings, AmbientLight>;

CreateObjectResult createAmbientLight(ObjectManager &objects, Device &device, const CreateAmbientLightParams &params);
GetAmbientLightResult getAmbientLight(ObjectManager &objects, const GetObjectParams &params);
void updateAmbientLight(ObjectManager &objects, Device &device, const UpdateAmbientLightParams &params);

template<>
struct JsonObjectReflector<SunSkyLightSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<SunSkyLightSettings>();
        builder.field("up", [](auto &object) { return &object.up; }).description("Zenith of the sky XYZ").defaultValue(Vector3{0.0F, 1.0F, 0.0F});
        builder.field("direction", [](auto &object) { return &object.direction; })
            .description("Sun direction XYZ")
            .defaultValue(Vector3{0.0F, -1.0F, 0.0F});
        builder.field("turbidity", [](auto &object) { return &object.turbidity; })
            .description("Atmospheric turbidity due to particles")
            .defaultValue(3.0F)
            .minimum(1)
            .maximum(10);
        builder.field("albedo", [](auto &object) { return &object.albedo; })
            .description("Ground reflectance")
            .defaultValue(0.3F)
            .minimum(0.0F)
            .maximum(1.0F);
        builder.field("horizonExtension", [](auto &object) { return &object.horizonExtension; })
            .description("Stetch horizon to this fraction of lower hemisphere")
            .defaultValue(0.01F)
            .minimum(0.0F)
            .maximum(1.0F);
        builder.field("intensityScale", [](auto &object) { return &object.intensityScale; })
            .description("Scale sun light intensity by this factor")
            .defaultValue(1.0F);
        return builder.build();
    }
};

using CreateSunSkyLightParams = CreateLightParamsOf<SunSkyLightSettings>;
using GetSunSkyLightResult = GetResultOf<SunSkyLightSettings>;
using UpdateSunSkyLightParams = UpdateParamsOf<SunSkyLightSettings>;
using UserSunSkyLight = UserLightOf<SunSkyLightSettings, SunSkyLight>;

CreateObjectResult createSunSkyLight(ObjectManager &objects, Device &device, const CreateSunSkyLightParams &params);
GetSunSkyLightResult getSunSkyLight(ObjectManager &objects, const GetObjectParams &params);
void updateSunSkyLight(ObjectManager &objects, Device &device, const UpdateSunSkyLightParams &params);
}
