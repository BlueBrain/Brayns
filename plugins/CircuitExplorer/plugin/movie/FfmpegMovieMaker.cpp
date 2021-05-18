/* Copyright (c) 2021, EPFL/Blue Brain Project
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

#include "MovieMaker.h"

#include <sstream>

#include "ffmpeg/Movie.h"

namespace
{
class FfmpegMovieMaker
{
public:
    static void createMovie(const MovieInfo& info)
    {
        auto ffmpegInfo = _createMovieInfo(info);
        ffmpeg::Movie movie(ffmpegInfo);
        movie.open(info.outputFile);
        for (const auto& frame : info.inputFiles)
        {
            movie.addFrame(frame);
        }
        movie.close();
    }

private:
    static ffmpeg::VideoInfo _createMovieInfo(const MovieInfo& movie)
    {
        ffmpeg::VideoInfo info;
        info.codec = getCodec(movie);
        info.width = movie.width;
        info.height = movie.height;
        info.format = getFormat(movie);
        info.bitrate = getBitrate(movie);
        info.framerate = movie.framerate;
        return info;
    }

    static int64_t getBitrate(const MovieInfo& info)
    {
        return info.bitrate <= 0
                   ? int64_t(3) * int64_t(info.width) * int64_t(info.height) *
                         int64_t(info.framerate)
                   : info.bitrate;
    }

    static AVCodec* getCodec(const MovieInfo& info)
    {
        return ffmpeg::VideoInfo::getCodec(info.codec.empty() ? "libx264"
                                                              : info.codec);
    }

    static AVPixelFormat getFormat(const MovieInfo& info)
    {
        return ffmpeg::VideoInfo::getFormat(info.format.empty() ? "yuv420p"
                                                                : info.format);
    }
};
} // namespace

void MovieMaker::createMovie(const MovieInfo& info)
{
    try
    {
        FfmpegMovieMaker::createMovie(info);
    }
    catch (const ffmpeg::Exception& e)
    {
        std::ostringstream message;
        message << "FFmpeg error: '" << e.getMessage()
                << "' (code: " << e.getStatusCode() << " '"
                << e.getStatusDescription() << "')";
        throw MovieMakerException(message.str());
    }
}