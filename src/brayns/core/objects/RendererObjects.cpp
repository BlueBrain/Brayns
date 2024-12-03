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

#include "RendererObjects.h"

#include <fmt/format.h>

#include "common/Objects.h"

namespace
{
using namespace brayns;

BackgroundParams getBackgroundParams(const Stored<UserTexture> &background)
{
    return BackgroundTexture{background.getId()};
}

BackgroundParams getBackgroundParams(const auto &background)
{
    return background;
}

BackgroundParams getBackgroundParams(const BackgroundStorage &background)
{
    return std::visit([](const auto &value) { return getBackgroundParams(value); }, background);
}

RendererParams getRendererParams(const RendererStorage &storage)
{
    return {
        .value = storage.settings,
        .materials = getObjectIds(storage.materials),
        .background = getBackgroundParams(storage.background),
        .maxDepth = storage.maxDepth ? storage.maxDepth->getId() : std::optional<ObjectId>(),
    };
}

Background getBackground(const Stored<UserTexture> &background)
{
    auto &texture = castTextureAs<UserTexture2D>(background);
    return texture.value;
}

Background getBackground(const auto &background)
{
    return background;
}

Background getBackground(const BackgroundStorage &background)
{
    return std::visit([](const auto &value) { return getBackground(value); }, background);
}

BackgroundStorage getBackgroundStorage(ObjectManager &objects, const BackgroundTexture &background)
{
    auto texture = objects.getAsStored<UserTexture>(background.texture2d);
    return texture;
}

BackgroundStorage getBackgroundStorage(ObjectManager &objects, const auto &background)
{
    (void)objects;
    return background;
}

BackgroundStorage getBackgroundStorage(ObjectManager &objects, const BackgroundParams &background)
{
    return std::visit([&](const auto &value) { return getBackgroundStorage(objects, value); }, background);
}

RendererStorage getRendererStorage(ObjectManager &objects, const RendererParams &params)
{
    auto materials = getStoredObjects<UserMaterial>(objects, params.materials);
    auto background = getBackgroundStorage(objects, params.background);
    auto maxDepth = params.maxDepth ? objects.getAsStored<UserTexture>(*params.maxDepth) : std::optional<Stored<UserTexture>>();

    auto settings = params.value;

    settings.materials = getObjectHandles(materials);
    settings.background = getBackground(background);
    settings.maxDepth = maxDepth ? getTextureFrom<UserTexture2D>(*maxDepth) : std::optional<Texture2D>();

    return {
        .settings = std::move(settings),
        .materials = std::move(materials),
        .background = std::move(background),
        .maxDepth = std::move(maxDepth),
    };
}

template<typename T>
void checkAllMaterialsAre(const std::vector<Stored<UserMaterial>> &materials)
{
    for (const auto &material : materials)
    {
        if (material.get().value.type() != typeid(T))
        {
            throw InvalidParams(fmt::format("Material type '{}' is not supported by this renderer type", material.getType()));
        }
    }
}

template<typename T>
auto createRendererAs(ObjectManager &objects, const auto &params, std::string type, auto &&validate, auto &&create)
{
    const auto &[objectParams, rendererParams] = params;
    const auto &[base, derived] = rendererParams;

    auto storage = getRendererStorage(objects, base);

    validate(storage);

    auto renderer = create(storage.settings, derived);

    auto ptr = toShared(T{derived, std::move(renderer)});

    auto object = UserRenderer{
        .storage = std::move(storage),
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {std::move(type)}, objectParams);

    return getResult(stored);
}

template<typename T>
auto getRendererAs(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserRenderer>(params.id);
    auto &renderer = *castAsShared<T>(object.get().value, object);
    return getResult(renderer.storage);
}

template<typename T, typename U>
void updateRendererAs(ObjectManager &objects, Device &device, const UpdateParamsOf<U> &params)
{
    auto object = objects.getAsStored<UserRenderer>(params.id);
    auto &renderer = *castAsShared<T>(object.get().value, object);

    auto settings = getUpdatedParams(params, renderer.storage);

    renderer.value.update(settings);
    device.throwIfError();

    renderer.storage = settings;
}
}

namespace brayns
{
GetRendererResult getRenderer(ObjectManager &objects, const GetObjectParams &params)
{
    auto &renderer = objects.getAs<UserRenderer>(params.id);
    auto result = getRendererParams(renderer.storage);
    return getResult(result);
}

void updateRenderer(ObjectManager &objects, Device &device, const UpdateRendererParams &params)
{
    auto &renderer = objects.getAs<UserRenderer>(params.id);

    auto current = getRendererParams(renderer.storage);
    auto updated = getUpdatedParams(params, current);

    auto storage = getRendererStorage(objects, updated);

    renderer.get().update(storage.settings);
    device.throwIfError();

    renderer.storage = std::move(storage);
}

CreateObjectResult createAoRenderer(ObjectManager &objects, Device &device, const CreateAoRendererParams &params)
{
    auto validate = [](const auto &storage) { checkAllMaterialsAre<UserAoMaterial>(storage.materials); };
    auto create = [&](const auto &base, const auto &derived) { return createAoRenderer(device, base, derived); };
    return createRendererAs<UserAoRenderer>(objects, params, "AoRenderer", validate, create);
}

GetAoRendererResult getAoRenderer(ObjectManager &objects, const GetObjectParams &params)
{
    return getRendererAs<UserAoRenderer>(objects, params);
}

void updateAoRenderer(ObjectManager &objects, Device &device, const UpdateAoRendererParams &params)
{
    updateRendererAs<UserAoRenderer>(objects, device, params);
}

CreateObjectResult createScivisRenderer(ObjectManager &objects, Device &device, const CreateScivisRendererParams &params)
{
    auto validate = [](const auto &storage) { checkAllMaterialsAre<UserScivisMaterial>(storage.materials); };
    auto create = [&](const auto &base, const auto &derived) { return createScivisRenderer(device, base, derived); };
    return createRendererAs<UserScivisRenderer>(objects, params, "ScivisRenderer", validate, create);
}

GetScivisRendererResult getScivisRenderer(ObjectManager &objects, const GetObjectParams &params)
{
    return getRendererAs<UserScivisRenderer>(objects, params);
}

void updateScivisRenderer(ObjectManager &objects, Device &device, const UpdateScivisRendererParams &params)
{
    updateRendererAs<UserScivisRenderer>(objects, device, params);
}

CreateObjectResult createPathTracer(ObjectManager &objects, Device &device, const CreatePathTracerParams &params)
{
    auto validate = [](const auto &storage) { checkAllMaterialsAre<PrincipledMaterial>(storage.materials); };
    auto create = [&](const auto &base, const auto &derived) { return createPathTracer(device, base, derived); };
    return createRendererAs<UserPathTracer>(objects, params, "PathTracer", validate, create);
}

GetPathTracerResult getPathTracer(ObjectManager &objects, const GetObjectParams &params)
{
    return getRendererAs<UserPathTracer>(objects, params);
}

void updatePathTracer(ObjectManager &objects, Device &device, const UpdatePathTracerParams &params)
{
    updateRendererAs<UserPathTracer>(objects, device, params);
}
}
