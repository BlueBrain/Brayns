/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/core/Engine.h>
#include <brayns/engine/json/adapters/ViewAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class SetCameraLookAtEntrypoint final : public Entrypoint<View, EmptyMessage>
{
public:
    SetCameraLookAtEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
    void onRequest(const Request &request) override;

private:
    Engine &_engine;
};

class GetCameraLookAtEntrypoint final : public Entrypoint<EmptyMessage, View>
{
public:
    GetCameraLookAtEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
    void onRequest(const Request &request) override;

private:
    Engine &_engine;
};
}
