/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <string>
#include <string_view>
#include <vector>

#include <brayns/common/geometry/TriangleMesh.h>

namespace brayns
{
/**
 * @brief Generic interface to define a mesh parser.
 *
 */
class MeshParser
{
public:
    virtual ~MeshParser() = default;

    /**
     * @brief Get a list of the file extensions supported by the parser.
     *
     * @return std::vector<std::string> List of supported file extensions.
     */
    virtual std::vector<std::string> getSupportedExtensions() const = 0;

    /**
     * @brief Parse the raw file data and return the mesh.
     *
     * @param data File data in text or binary format.
     * @return TriangleMesh Extracted mesh.
     */
    virtual TriangleMesh parse(std::string_view data) const = 0;
};
} // namespace brayns
