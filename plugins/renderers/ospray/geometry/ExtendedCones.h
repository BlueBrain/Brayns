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
    int64 offset_centerRadius;
    int64 offset_upRadius;
    int64 offset_frame;
    int64 offset_materialID;

    ospray::Ref<ospray::Data> data;

    ExtendedCones();
};

} // ::brayns
