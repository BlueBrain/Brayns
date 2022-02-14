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

#include "VolumeParametersEntrypoint.h"

namespace brayns
{
GetVolumeParametersEntrypoint::GetVolumeParametersEntrypoint(const VolumeParameters &parameters, INetworkInterface &interface)
    : GetEntrypoint(parameters, interface)
{
}

std::string GetVolumeParametersEntrypoint::getName() const
{
    return "get-volume-parameters";
}

std::string GetVolumeParametersEntrypoint::getDescription() const
{
    return "Get the current state of the volume parameters";
}

SetVolumeParametersEntrypoint::SetVolumeParametersEntrypoint(VolumeParameters &parameters, Engine &engine)
    : SetEntrypoint(parameters, engine)
{
}

std::string SetVolumeParametersEntrypoint::getName() const
{
    return "set-volume-parameters";
}

std::string SetVolumeParametersEntrypoint::getDescription() const
{
    return "Set the current state of the volume parameters";
}
} // namespace brayns
