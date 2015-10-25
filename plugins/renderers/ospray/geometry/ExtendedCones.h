/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include "ospray/geometry/Geometry.h"

namespace brayns
{
struct ExtendedCones : public ospray::Geometry
{
    std::string toString() const final { return "ospray::Cones"; }
    void finalize(ospray::Model *model) final;

    float radius;
    float length;
    int32 materialID;

    size_t numExtendedCones;
    size_t bytesPerCone;
    int64 offset_center;
    int64 offset_up;
    int64 offset_radius;
    int64 offset_length;
    int64 offset_frame;
    int64 offset_materialID;

    ospray::Ref<ospray::Data> data;

    ExtendedCones();
};

} // ::brayns
