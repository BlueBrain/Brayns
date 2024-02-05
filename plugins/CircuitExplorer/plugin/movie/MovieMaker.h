/* Copyright 2021-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief This struct holds the information required from the user to create
 * a movie from a set of images.
 */
struct MovieInfo
{
    /**
     * @brief Output video file path (ex: /home/myvideos/movie.mp4)
     */
    std::string outputFile;

    /**
     * @brief List of pathes of the frame used to make the movie (ex:
     * {"/frame1.png", "/frame2.png"}).
     * @note The frames will be presented in the same order as provided.
     */
    std::vector<std::string> inputFiles;

    /**
     * @brief The width of the output video in pixels.
     */
    int width = 0;

    /**
     * @brief The height of the output video in pixels.
     */
    int height = 0;

    /**
     * @brief The framerate of the output video in frame per second.
     * @note Movie duration = number of inputFiles / framerate.
     */
    int framerate = 0;

    /**
     * @brief The bitrate of the video encoding.
     * @note If left as zero, an appropriate bitrate will be computed based
     * on the other parameters.
     */
    int64_t bitrate = 0;

    /**
     * @brief The name of the codec to use to encode the output video.
     * @note If left empty, it will be set to "libx264".
     */
    std::string codec;

    /**
     * @brief The name of the pixel format of the output video.
     * @note If left empty, it will be set to "yuv420p".
     */
    std::string format;
};

/**
 * @brief This exception will be thrown if an error occurs during the movie
 * generation that prevents the movie from being created.
 * @note Can be catched to check if the movie generation was successful and get
 * a description of the error.
 */
class MovieCreationException : public std::runtime_error
{
public:
    /**
     * @brief Construct an exception from the error message.
     */
    MovieCreationException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

/**
 * @brief The MovieMaker class creates a movie file from a set of image files.
 * User can specify the video parameters using the MovieInfo class.
 */
class MovieMaker
{
public:
    /**
     * @brief Create a movie based on the output file provided by the user.
     * @param info Information provided by the user to create the movie.
     * @throw MovieCreationException if the movie cannot be created.
     */
    static void createMovie(const MovieInfo& info);
};