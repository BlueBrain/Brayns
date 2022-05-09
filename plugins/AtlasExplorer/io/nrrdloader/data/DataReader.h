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

#include <io/nrrdloader/NRRDHeader.h>

#include <string_view>
#include <vector>

class DataReader
{
public:
    /**
     * @brief Reads the NRRD volume contents into a contiguous byte array. If data files entry is present in the
     * header, it will attempt to read the files. Otherwise, it will attempt to extract them from the remaining data
     * content of the source NRRD file.
     *
     * @param header The NRRD header to configure the read
     * @param fileName The path to the NRRD file
     * @param dataContent The content of the main nrrd file
     * @return std::vector<uint8_t>
     */
    static std::vector<uint8_t>
        read(const NRRDHeader &header, const std::string &fileName, std::string_view dataContent);
};
