/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#pragma once

#include <api/IUseCase.h>

#include <brayns/json/JsonObjectMacro.h>

enum class LayerDistanceType
{
    lower,
    upper
};

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(LayerDistanceType, {"lower", LayerDistanceType::lower}, {"upper", LayerDistanceType::upper})
}

BRAYNS_JSON_OBJECT_BEGIN(LayerDistanceParameters)
BRAYNS_JSON_OBJECT_ENTRY(LayerDistanceType, type, "Type of distance to show")
BRAYNS_JSON_OBJECT_END()

class LayerDistance final : public IUseCase
{
public:
    std::string getName() const override;
    bool isValidAtlas(const Atlas &atlas) const override;
    std::shared_ptr<brayns::Model> run(const Atlas &atlas, const brayns::JsonValue &payload) const override;
};
