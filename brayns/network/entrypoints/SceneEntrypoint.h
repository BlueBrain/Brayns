/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/entrypoint/ObjectEntrypoint.h>
#include <brayns/network/messages/SceneMessage.h>

namespace brayns
{
class GetSceneEntrypoint : public GetEntrypoint<SceneMessage>
{
public:
    virtual std::string getName() const override { return "get-scene"; }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the scene";
    }
};

class SetSceneEntrypoint : public SetEntrypoint<SceneMessage>
{
public:
    virtual std::string getName() const override { return "set-scene"; }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the scene";
    }
};
} // namespace brayns