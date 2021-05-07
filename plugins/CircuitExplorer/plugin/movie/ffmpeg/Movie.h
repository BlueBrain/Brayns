#pragma once

#include <sstream>
#include <string>

#include "Exception.h"
#include "Status.h"
#include "VideoInfo.h"
#include "conversion/ImageConverter.h"
#include "input/ImageReader.h"
#include "output/VideoStream.h"

namespace ffmpeg
{
class Movie
{
public:
    Movie(const VideoInfo& info)
        : _stream(info)
    {
        _conversionInfo.width = info.width;
        _conversionInfo.height = info.height;
        _conversionInfo.format = info.format;
    }

    Movie(const Movie&) = delete;
    Movie& operator=(const Movie&) = delete;

    void open(const std::string& filename)
    {
        try
        {
            _stream.open(filename);
        }
        catch (const Exception& e)
        {
            std::ostringstream message;
            message << "Error while opening output file '" << filename
                    << "': " << e.getMessage();
            throw Exception(message.str(), e.getStatus());
        }
    }

    void addFrame(const std::string& filename)
    {
        try
        {
            auto image = ImageReader::read(filename);
            auto frame = ImageConverter::convert(image.get(), _conversionInfo);
            _stream.write(frame.get());
        }
        catch (const Exception& e)
        {
            std::ostringstream message;
            message << "Error while processing frame '" << filename
                    << "': " << e.getMessage();
            throw Exception(message.str(), e.getStatus());
        }
    }

    void close()
    {
        try
        {
            _stream.close();
        }
        catch (const Exception& e)
        {
            std::ostringstream message;
            message << "Error while closing output file '"
                    << "': " << e.getMessage();
            throw Exception(message.str(), e.getStatus());
        }
    }

private:
    ConversionInfo _conversionInfo;
    VideoStream _stream;
};
} // namespace ffmpeg