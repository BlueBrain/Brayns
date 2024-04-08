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

#include "AddClippingGeometryEntrypoint.h"

namespace brayns
{
AddClippingBoundedPlanesEntrypoint::AddClippingBoundedPlanesEntrypoint(ModelManager &models):
    AddClippingGeometryEntrypoint<BoundedPlane>(models)
{
}

std::string AddClippingBoundedPlanesEntrypoint::getMethod() const
{
    return "add-clipping-bounded-planes";
}

std::string AddClippingBoundedPlanesEntrypoint::getDescription() const
{
    return "Add a list of axis-aligned bound limited clipping planes";
}

AddClippingBoxesEntrypoint::AddClippingBoxesEntrypoint(ModelManager &models):
    AddClippingGeometryEntrypoint<Box>(models)
{
}

std::string AddClippingBoxesEntrypoint::getMethod() const
{
    return "add-clipping-boxes";
}

std::string AddClippingBoxesEntrypoint::getDescription() const
{
    return "Add a list of clipping boxes to the scene";
}

AddClippingPlanesEntrypoint::AddClippingPlanesEntrypoint(ModelManager &models):
    AddClippingGeometryEntrypoint<Plane>(models)
{
}

std::string AddClippingPlanesEntrypoint::getMethod() const
{
    return "add-clipping-planes";
}

std::string AddClippingPlanesEntrypoint::getDescription() const
{
    return "Add a list of clipping planes to the scene";
}

AddClippingCapsulesEntrypoint::AddClippingCapsulesEntrypoint(ModelManager &models):
    AddClippingGeometryEntrypoint<Capsule>(models)
{
}

std::string AddClippingCapsulesEntrypoint::getMethod() const
{
    return "add-clipping-capsules";
}

std::string AddClippingCapsulesEntrypoint::getDescription() const
{
    return "Add a list of clipping capsules to the scene";
}

AddClippingSpheresEntrypoint::AddClippingSpheresEntrypoint(ModelManager &models):
    AddClippingGeometryEntrypoint<Sphere>(models)
{
}

std::string AddClippingSpheresEntrypoint::getMethod() const
{
    return "add-clipping-spheres";
}

std::string AddClippingSpheresEntrypoint::getDescription() const
{
    return "Add a list of clipping spheres to the scene";
}
}
