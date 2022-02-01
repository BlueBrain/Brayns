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

#include "MakeMovieEntrypoint.h"

#include <algorithm>
#include <filesystem>

#include <plugin/movie/MovieMaker.h>

namespace
{
class MovieHelper
{
public:
    static void makeMovie(const MakeMovieMessage &params)
    {
        // The folder the frames image files are stored in
        auto &frameFolder = params.frames_folder_path;

        // Return an error if the directory provided doesn't exist
        if (!std::filesystem::is_directory(frameFolder))
        {
            throw brayns::EntrypointException("Invalid frame folder: '" + frameFolder + "'");
        }

        // Make sure the extension has a dot as in boost::filesystem
        std::string extension = params.frames_file_extension;
        if (!extension.empty() && extension[0] != '.')
        {
            extension.insert(extension.begin(), '.');
        }

        // Extract video frames from folder and extension
        std::vector<std::string> frames;
        for (const auto &entry : std::filesystem::directory_iterator(frameFolder))
        {
            if (!std::filesystem::is_regular_file(entry))
            {
                continue;
            }
            auto &path = entry.path();
            if (path.extension() != extension)
            {
                continue;
            }
            frames.push_back(path.native());
        }

        // Use the name to guess frame position in the video (ex:
        // frame_0001.png)
        std::sort(frames.begin(), frames.end());

        // Fill movie parameters for the movie maker
        MovieInfo movie;
        movie.outputFile = params.output_movie_path;
        movie.inputFiles = std::move(frames);
        movie.width = params.dimensions[0];
        movie.height = params.dimensions[1];
        movie.framerate = params.fps_rate;

        // Create the movie and report any errors
        try
        {
            MovieMaker::createMovie(movie);
        }
        catch (const MovieCreationException &e)
        {
            throw brayns::EntrypointException(e.what());
        }

        // Remove frames image files from disk if asked to save space
        if (params.erase_frames)
        {
            for (const auto &frame : movie.inputFiles)
            {
                std::filesystem::remove(frame);
            }
        }
    }
};
} // namespace

std::string MakeMovieEntrypoint::getName() const
{
    return "make-movie";
}

std::string MakeMovieEntrypoint::getDescription() const
{
    return "Builds a movie file from a set of frames stored on disk";
}

void MakeMovieEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    MovieHelper::makeMovie(params);
    request.reply(brayns::EmptyMessage());
}
