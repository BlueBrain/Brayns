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

#include <brayns/network/json/Message.h>

BRAYNS_MESSAGE_BEGIN(MakeMovieMessage)
BRAYNS_MESSAGE_ENTRY(brayns::Vector2ui, dimensions,
                     "Video dimensions (width,height)")
BRAYNS_MESSAGE_ENTRY(std::string, frames_folder_path,
                     "Path to where to fetch the frames to create the video")
BRAYNS_MESSAGE_ENTRY(std::string, frames_file_extension,
                     "The extension of the frame files to fetch (ex: png, jpg)")
BRAYNS_MESSAGE_ENTRY(uint32_t, fps_rate,
                     "The frames per second rate at which to create the video")
BRAYNS_MESSAGE_ENTRY(std::string, output_movie_path,
                     "The path to where the movie will be created."
                     " Must include filename and extension")
BRAYNS_MESSAGE_ENTRY(bool, erase_frames,
                     "Wether to clean up the frame image files after generating"
                     " the video file")
BRAYNS_MESSAGE_END()