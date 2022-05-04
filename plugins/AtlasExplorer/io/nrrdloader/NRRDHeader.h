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

#include <brayns/engine/VolumeDataType.h>

#include <optional>

enum NRRDEncoding
{
    RAW,
    ASCII,
    HEX,
    GZIP,
    BZIP2
};

enum NRRDEndianness
{
    LITTLE,
    BIG
};

struct NRRDHeader
{
    // Data dimensions
    int32_t dimensions{-1};

    // Data type per voxel
    brayns::VolumeDataType type;

    // Data encoding format
    NRRDEncoding encoding;

    // Endianness (only needed for raw, hex, gzip and bzip2, if datatype size is greater than 1)
    std::optional<NRRDEndianness> endian;

    // Descriptive text about the content of the data
    std::optional<std::string> content;

    // Min value found in the data (any type)
    std::optional<double> min;

    // Max value found in the data (any type)
    std::optional<double> max;

    // If the data is detached from the header, path to the data file(s)
    std::optional<std::vector<std::string>> dataFiles;

    // Units of the data scalar values themselves
    std::optional<std::string> sampleUnits;

    // Number of samples on each dimension
    std::vector<int32_t> sizes;

    // Determines how many components there are in the space origin and space directions fields
    std::optional<int32_t> spaceDimensions;

    // Units in which space vectors are measured
    std::optional<std::vector<std::string>> spaceUnits;

    // World space center position of the first voxel
    std::optional<std::vector<float>> spaceOrigin;

    // Directionality and world spacing of the volume grid
    std::optional<std::vector<std::vector<float>>> spaceDirections;

    // Rotation matrix (column-major)
    std::optional<std::vector<std::vector<float>>> measurementFrame;
};
