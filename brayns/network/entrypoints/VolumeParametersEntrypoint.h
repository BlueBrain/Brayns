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
#include <brayns/network/messages/VolumeParametersAdapter.h>

#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
class GetVolumeParametersEntrypoint : public GetEntrypoint<VolumeParameters>
{
public:
    virtual std::string getName() const override
    {
        return "get-volume-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the volume parameters";
    }
};

class SetVolumeParametersEntrypoint : public SetEntrypoint<VolumeParameters>
{
public:
    virtual std::string getName() const override
    {
        return "set-volume-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the volume parameters";
    }
};
} // namespace brayns