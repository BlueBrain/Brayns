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

#include <api/DataMangler.h>
#include <io/nrrdloader/NRRDHeader.h>

#include <memory>
#include <string_view>
#include <vector>

class DataParser
{
public:
    /**
     * @brief Parses the NRRD contents
     *
     * @param header The NRRD Header to configure the read
     * @param content The left over content from the NRRD file after extracting the header
     * @return std::unique_ptr<IDataMangler>
     */
    static std::unique_ptr<IDataMangler> parse(const NRRDHeader &header, std::string_view content);
};
