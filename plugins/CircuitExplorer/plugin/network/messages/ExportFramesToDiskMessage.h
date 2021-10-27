/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
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

#include <brayns/json/Message.h>

#include <plugin/network/adapters/ODUCameraAdapter.h>

BRAYNS_MESSAGE_BEGIN(ExportFramesToDisk)
BRAYNS_MESSAGE_ENTRY(std::string, path,
                     "Path to the directory where the frames will be saved")
BRAYNS_MESSAGE_ENTRY(std::string, format, "The image format (PNG or JPEG)")
BRAYNS_MESSAGE_ENTRY(bool, nameAfterStep,
                     "Name the file on disk after the simulation step index")
BRAYNS_MESSAGE_ENTRY(uint32_t, quality,
                     "The quality at which the images will be stored")
BRAYNS_MESSAGE_ENTRY(
    uint32_t, spp,
    "Samples per pixels (The more, the better visual result and "
    "the slower the"
    " rendering)")
BRAYNS_MESSAGE_ENTRY(std::vector<uint64_t>, animationInformation,
                     "A list of frame numbers to render")
BRAYNS_MESSAGE_ENTRY(
    std::vector<ODUCameraInformation>, cameraInformation,
    "A list of camera definitions, one for each exported frame.")
BRAYNS_MESSAGE_END()
