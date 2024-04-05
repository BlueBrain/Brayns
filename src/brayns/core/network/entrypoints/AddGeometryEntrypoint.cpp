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

#include "AddGeometryEntrypoint.h"

namespace brayns
{
AddBoundedPlanesEntrypoint::AddBoundedPlanesEntrypoint(ModelManager &models):
    AddGeometryEntrypoint<BoundedPlane>(models)
{
}

std::string AddBoundedPlanesEntrypoint::getMethod() const
{
    return "add-bounded-planes";
}

std::string AddBoundedPlanesEntrypoint::getDescription() const
{
    return "Adds a list of axis-aligned bound limited planes";
}

AddBoxesEntrypoint::AddBoxesEntrypoint(ModelManager &models):
    AddGeometryEntrypoint<Box>(models)
{
}

std::string AddBoxesEntrypoint::getMethod() const
{
    return "add-boxes";
}

std::string AddBoxesEntrypoint::getDescription() const
{
    return "Adds a list of boxes to the scene";
}

AddPlanesEntrypoint::AddPlanesEntrypoint(ModelManager &models):
    AddGeometryEntrypoint<Plane>(models)
{
}

std::string AddPlanesEntrypoint::getMethod() const
{
    return "add-planes";
}

std::string AddPlanesEntrypoint::getDescription() const
{
    return "Adds a list of planes to the scene";
}

AddCapsulesEntrypoint::AddCapsulesEntrypoint(ModelManager &models):
    AddGeometryEntrypoint<Capsule>(models)
{
}

std::string AddCapsulesEntrypoint::getMethod() const
{
    return "add-capsules";
}

std::string AddCapsulesEntrypoint::getDescription() const
{
    return "Adds a list of capsules to the scene";
}

AddSpheresEntrypoint::AddSpheresEntrypoint(ModelManager &models):
    AddGeometryEntrypoint<Sphere>(models)
{
}

std::string AddSpheresEntrypoint::getMethod() const
{
    return "add-spheres";
}

std::string AddSpheresEntrypoint::getDescription() const
{
    return "Adds a list of spheres to the scene";
}
}
