/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "../../common/types.h"

#include <brayns/common/types.h>
#include <brayns/network/json/ActionMessage.h>

struct SynapseAttributes : public brayns::Message
{
    MESSAGE_BEGIN(SynapseAttributes)
    MESSAGE_ENTRY(std::string, circuitConfiguration,
                  "Path to the circuit configuration file")
    MESSAGE_ENTRY(int32_t, gid, "Target cell GID")
    MESSAGE_ENTRY(std::vector<std::string>, htmlColors,
                  "List of rgb colors in hexadecimal")
    MESSAGE_ENTRY(double, lightEmission,
                  "Emission parameter for the synapse material")
    MESSAGE_ENTRY(double, radius, "Synapse geometry radius")
};

struct CameraDefinition : public brayns::Message
{
    MESSAGE_BEGIN(CameraDefinition)
    MESSAGE_ENTRY(std::vector<double>, origin, "The position of the camera")
    MESSAGE_ENTRY(std::vector<double>, direction,
                  "A normalized vector in the direction the camera is facing")
    MESSAGE_ENTRY(std::vector<double>, up,
                  "A normalized vector, perpendicular to the direction, that "
                  "points to the camera"
                  " upwards")
    MESSAGE_ENTRY(double, apertureRadius, "The camera aperture")
    MESSAGE_ENTRY(double, focusDistance,
                  "The distance from the origin, in the direction, at which "
                  "the camera will focus")
};

struct ExportFramesToDisk : public brayns::Message
{
    MESSAGE_BEGIN(ExportFramesToDisk)
    MESSAGE_ENTRY(std::string, path,
                  "Path to the directory where the frames will be saved")
    MESSAGE_ENTRY(std::string, format, "The image format (PNG or JPEG)")
    MESSAGE_ENTRY(bool, nameAfterStep,
                  "Name the file on disk after the simulation step index")
    MESSAGE_ENTRY(uint32_t, quality,
                  "The quality at which the images will be stored")
    MESSAGE_ENTRY(uint32_t, spp,
                  "Samples per pixels (The more, the better visual result and "
                  "the slower the"
                  " rendering)")
    MESSAGE_ENTRY(uint32_t, startFrame,
                  "The frame at which to start exporting frames")
    MESSAGE_ENTRY(std::vector<uint64_t>, animationInformation,
                  "A list of frame numbers to render")
    MESSAGE_ENTRY(
        std::vector<double>, cameraInformation,
        "A list of camera definitions. Each camera definition contains origin, "
        "direction, up, apperture and radius. (1 entry per animation "
        "information entry)")
};

struct FrameExportProgress : public brayns::Message
{
    MESSAGE_BEGIN(FrameExportProgress)
    MESSAGE_ENTRY(double, progress,
                  "The normalized progress (0.0 to 1.0) of the last export "
                  "frames to disk request")
};
