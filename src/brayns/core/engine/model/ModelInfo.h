/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/core/engine/components/LoadInfo.h>
#include <brayns/core/engine/components/Metadata.h>
#include <brayns/core/engine/components/Transform.h>

#include "Model.h"

namespace brayns
{
class ModelInfo
{
public:
    explicit ModelInfo(const Model &model);

    const LoadInfo *getLoadInfo() const noexcept;
    const Metadata *getMetadata() const noexcept;
    const Transform *getBaseTransform() const noexcept;

private:
    const LoadInfo *_loadInfo;
    const Metadata *_metadata;
    const Transform *_baseTransform;
};
}
