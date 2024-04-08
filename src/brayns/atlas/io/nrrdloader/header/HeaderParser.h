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

#include <brayns/atlas/io/nrrdloader/NRRDHeader.h>

#include <string_view>

class HeaderParser
{
public:
    /**
     * @brief Parses the header of a nrrd file data. The input view is updated to point
     * to the first element (if any) after the header
     *
     * @param nrrdPath path to the nrrd file
     * @param nrrdContentView input view of the data. Is updated during the parse process
     * @return NRRDHeader
     */
    static NRRDHeader parse(std::string nrrdPath, std::string_view &nrrdContentView);
};
