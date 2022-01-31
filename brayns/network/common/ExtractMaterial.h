/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>

namespace brayns
{
/**
 * @brief Helper class to extract a material from a model with error handling.
 *
 */
class ExtractMaterial
{
public:
    /**
     * @brief Extract a material from a model descriptor and its ID.
     *
     * @param descriptor Source model.
     * @param id Material ID
     * @return Material& Material instance.
     * @throw EntrypointException Material or model not found.
     */
    static Material &fromId(ModelDescriptor &descriptor, size_t id);

    /**
     * @brief Extract a material from a model and its ID.
     *
     * @param model Source model.
     * @param id Material ID
     * @return Material& Material instance.
     * @throw EntrypointException Material or model not found.
     */
    static Material &fromId(Model &model, size_t modelId, size_t id);
};
} // namespace brayns
