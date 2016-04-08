/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#pragma once

#include <brayns/common/types.h>
#include "ospray/geometry/Geometry.h"

namespace brayns
{
struct ExtendedCylinders : public ospray::Geometry
{
    std::string toString() const final { return "ospray::Cylinders"; }
    void finalize(ospray::Model *model) final;

    float radius;
    int32 materialID;

    size_t numExtendedCylinders;
    size_t bytesPerCylinder;
    int64 offset_v0;
    int64 offset_v1;
    int64 offset_radius;
    int64 offset_frame;
    int64 offset_materialID;

    ospray::Ref<ospray::Data> data;

    ExtendedCylinders();
};

} // ::brayns
