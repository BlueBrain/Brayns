/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/Scene.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/network/adapters/MaterialAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/utils/StringUtils.h>

namespace brayns
{
template<typename MaterialUpdaterType>
class SetMaterialEntrypoint : public Entrypoint<MaterialUpdaterType, EmptyMessage>
{
public:
    SetMaterialEntrypoint(Scene& scene)
     : _scene(scene)
    {
    }

    void onRequest(const EntrypointRequest<MaterialUpdaterType, EmptyMessage>& request) override
    {
        auto params = request.getParams();
        params.updateMaterialOnModel(_scene);
        const auto result = Json::serialize(EmptyMessage());
        request.reply(result);
    }

private:
    Scene &_scene;
};

class SetDefaultMaterialEntrypoint final : public SetMaterialEntrypoint<DefaultMaterialUpdater>
{
public:
    SetDefaultMaterialEntrypoint(Scene &scene);
    std::string getMethod() const override;
    std::string getDescription() const override;
};
}
