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

#pragma once

#include <brayns/engine/model/Model.h>
#include <brayns/json/JsonSchema.h>

#include "Atlas.h"

class IUseCase
{
public:
    virtual ~IUseCase() = default;

    /**
     * @brief Returns the identification name of the use case.
     * @return std::string
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Returns the schema of the parameters accepted by the use case, if any.
     * @return brayns::JsonSchema
     */
    virtual brayns::JsonSchema getParamsSchema() const
    {
        return {};
    }

    /**
     * @brief Tests wether the use case can be applied to the atlas.
     * @param atlas Atlas to be tested.
     * @return true If the atlas is valid for this use case, false otherwise.
     */
    virtual bool isValidAtlas(const Atlas &atlas) const = 0;

    /**
     * @brief Applies the use case to the given atlas data and returns a new model with the result.
     * @param atlas The atlas to which to apply the use case
     * @param payload Optional parameters to configure the use case behaviour
     * @return std::shared_ptr<brayns::Model> The result of the use case.
     */
    virtual std::shared_ptr<brayns::Model> run(const Atlas &atlas, const brayns::JsonValue &payload) const = 0;
};
