/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/common/types.h>
#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
/**
 * Set all the properties from the current property map of the given object to
 * the given ospray object.
 */
void setOSPRayProperties(const PropertyObject& object, OSPObject ospObject);

/** Convert a brayns::Transformation to an osp::affine3f. */
osp::affine3f transformationToAffine3f(const Transformation& transformation);

/** Helper to add the given model as an instance to the given root model. */
void addInstance(OSPModel rootModel, OSPModel modelToAdd,
                 const Transformation& transform);
}
