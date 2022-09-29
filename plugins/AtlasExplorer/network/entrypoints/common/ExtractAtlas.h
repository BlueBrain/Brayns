/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include <brayns/engine/scene/ModelManager.h>

#include <api/AtlasVolume.h>
#include <components/AtlasData.h>

class ExtractAtlas
{
public:
    static const AtlasVolume &atlasFromId(brayns::ModelManager &models, uint32_t id);
    static const AtlasVolume &atlasFromModel(brayns::Model &model);
    static const AtlasData &componentFromId(brayns::ModelManager &models, uint32_t id);
    static const AtlasData &componentFromModel(brayns::Model &model);
};
