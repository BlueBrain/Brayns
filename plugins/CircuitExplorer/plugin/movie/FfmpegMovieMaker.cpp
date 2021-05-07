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
        return info.bitrate == 0
                   ? info.bitrate
                   : 3 * info.width * info.height * info.framerate;
    }

    static AVCodec* getCodec(const MovieInfo& info)
    {
        return ffmpeg::VideoInfo::getCodec(info.codec.empty() ? "libx264"
                                                              : info.codec);
    }

    static AVPixelFormat getFormat(const MovieInfo& info)
    {
        return ffmpeg::VideoInfo::getFormat(
            info.format.empty() ? "yuv420p" : info.format);
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