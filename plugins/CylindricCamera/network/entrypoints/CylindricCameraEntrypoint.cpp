/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "CylindricCameraEntrypoint.h"

SetCylindricCameraEntrypoint::SetCylindricCameraEntrypoint(brayns::Engine &engine):
    brayns::SetCameraEntrypoint<Cylindric>(engine)
{
}

std::string SetCylindricCameraEntrypoint::getMethod() const
{
    return "set-camera-cylindric";
}

std::string SetCylindricCameraEntrypoint::getDescription() const
{
    return "Sets the current camera to a cylindric one, with the specified parameters";
}

GetCylindricCameraEntrypoint::GetCylindricCameraEntrypoint(brayns::Engine &engine):
    brayns::GetCameraEntrypoint<Cylindric>(engine)
{
}

std::string GetCylindricCameraEntrypoint::getMethod() const
{
    return "get-camera-cylindric";
}

std::string GetCylindricCameraEntrypoint::getDescription() const
{
    return "Returns the current camera as cylindric";
}
