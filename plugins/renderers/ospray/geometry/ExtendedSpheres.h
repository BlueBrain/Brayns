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
struct ExtendedSpheres : public ospray::Geometry
{
    std::string toString() const final { return "hbp::ExtendedSpheres"; }
    void finalize(ospray::Model *model) final;

    float radius;
    int32 materialID;

    size_t numExtendedSpheres;
    size_t bytesPerExtendedSphere;
    int64 offset_center;
    int64 offset_radius;
    int64 offset_frame;
    int64 offset_materialID;

    ospray::Ref<ospray::Data> data;
    ospray::Ref<ospray::Data> materialList;

    ExtendedSpheres();

private:
    std::vector<void *> ispcMaterials_;

};

} // ::brayns

