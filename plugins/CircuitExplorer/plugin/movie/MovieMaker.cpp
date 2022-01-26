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

// This will not compile if FFmpeg is not installed
#ifdef BRAYNS_USE_FFMPEG

    #include <memory>
    #include <sstream>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libswscale/swscale.h>
}

// Private implementation
namespace
{
////////////////////////////////////////////////////////////////////////////////
// Utility classes used to report FFmpeg errors and allocate ressources.
////////////////////////////////////////////////////////////////////////////////

// Error handling

/**
 * @brief Wrapper around the error code returned by FFmpeg calls.
 */
class Status
{
public:
    Status() = default;

    Status(int code)
        : _code(code)
    {
    }

    int getCode() const
    {
        return _code;
    }

    bool isError() const
    {
        return _code < 0;
    }

    bool isSuccess() const
    {
        return _code == 0;
    }

    bool isRetry() const
    {
        return _code == AVERROR(EAGAIN) || _code == AVERROR_EOF;
    }

    std::string toString() const
    {
        char buffer[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(_code, buffer, sizeof(buffer));
        return buffer;
    }

    operator int() const
    {
        return _code;
    }

private:
    int _code = 0;
};

/**
 * @brief Exeption thrown when an error occurs in an FFmpeg function.
 * @note Used to avoid multiple error code checking and carry information about
 * error.
 */
class FfmpegException : public std::runtime_error
{
public:
    FfmpegException(const std::string &message)
        : std::runtime_error(message)
        , _message(message)
    {
    }

    FfmpegException(const std::string &message, Status status)
        : std::runtime_error(message)
        , _message(message)
        , _status(status)
    {
    }

    const std::string &getMessage() const
    {
        return _message;
    }
    Status getStatus() const
    {
        return _status;
    }
    int getStatusCode() const
    {
        return _status.getCode();
    }
    std::string getStatusDescription() const
    {
        return _status.toString();
    }

private:
    std::string _message;
    Status _status;
};

// Ressource management

/**
 * @brief Encoded frame
 */
class Packet
{
public:
    struct Deleter
    {
        void operator()(AVPacket *packet) const
        {
            av_packet_free(&packet);
        }
    };

    using Ptr = std::unique_ptr<AVPacket, Deleter>;

    static Ptr create()
    {
        auto packet = av_packet_alloc();
        if (!packet)
        {
            throw FfmpegException("Cannot allocate packet");
        }
        return Ptr(packet);
    }
};

using PacketPtr = Packet::Ptr;

/**
 * @brief Decoded frame
 */
class Frame
{
public:
    struct Deleter
    {
        void operator()(AVFrame *frame) const
        {
            av_frame_free(&frame);
        }
    };

    using Ptr = std::unique_ptr<AVFrame, Deleter>;

    static Ptr create()
    {
        auto frame = av_frame_alloc();
        if (!frame)
        {
            throw FfmpegException("Cannot allocate frame");
        }
        return Ptr(frame);
    }
};

using FramePtr = Frame::Ptr;

/**
 * @brief Context used to read or write files through FFmpeg
 */
class FormatContext
{
public:
    struct Deleter
    {
        void operator()(AVFormatContext *context) const
        {
            avformat_free_context(context);
        }
    };

    using Ptr = std::unique_ptr<AVFormatContext, Deleter>;
};

using FormatContextPtr = FormatContext::Ptr;

/**
 * @brief Context used to decode or encode images through FFmpeg
 */
class CodecContext
{
public:
    struct Deleter
    {
        void operator()(AVCodecContext *context) const
        {
            avcodec_free_context(&context);
        }
    };

    using Ptr = std::unique_ptr<AVCodecContext, Deleter>;

    static Ptr create(AVCodec *codec)
    {
        auto context = avcodec_alloc_context3(codec);
        if (!context)
        {
            throw FfmpegException("Cannot allocate codec context");
        }
        return Ptr(context);
    }
};

using CodecContextPtr = CodecContext::Ptr;

////////////////////////////////////////////////////////////////////////////////
// Image extraction
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Create an input context to read an image file.
 */
class InputContext
{
public:
    static FormatContextPtr create(const std::string &filename)
    {
        auto context = _openInput(filename);
        _findStreamInfo(context.get());
        return context;
    }

private:
    static FormatContextPtr _openInput(const std::string &filename)
    {
        AVFormatContext *context = nullptr;
        Status status = avformat_open_input(&context, filename.c_str(), nullptr, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot open input context", status);
        }
        return FormatContextPtr(context);
    }

    static void _findStreamInfo(AVFormatContext *context)
    {
        Status status = avformat_find_stream_info(context, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot find input stream info", status);
        }
    }
};

/**
 * @brief Find the video stream from an input context.
 */
class InputVideoStream
{
public:
    static AVCodecParameters *findCodec(AVFormatContext *context)
    {
        return find(context)->codecpar;
    }

    static AVStream *find(AVFormatContext *context)
    {
        auto stream = tryFind(context);
        if (!stream)
        {
            throw FfmpegException("Cannot find video stream");
        }
        return stream;
    }

    static AVStream *tryFind(AVFormatContext *context)
    {
        for (unsigned int i = 0; i < context->nb_streams; ++i)
        {
            auto stream = context->streams[i];
            if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                return stream;
            }
        }
        return nullptr;
    }
};

/**
 * @brief Manage the creation of the codec context required to decode an image.
 */
class DecoderContext
{
public:
    static CodecContextPtr create(AVCodecParameters *parameters)
    {
        auto codec = _findDecoder(parameters);
        auto context = CodecContext::create(codec);
        _prepareContext(context.get(), parameters);
        _openContext(context.get(), codec);
        return context;
    }

private:
    static AVCodec *_findDecoder(AVCodecParameters *parameters)
    {
        auto codec = avcodec_find_decoder(parameters->codec_id);
        if (!codec)
        {
            throw FfmpegException("Cannot find decoder");
        }
        return codec;
    }

    static void _prepareContext(AVCodecContext *context, AVCodecParameters *parameters)
    {
        Status status = avcodec_parameters_to_context(context, parameters);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot fill decoder context", status);
        }
    }

    static void _openContext(AVCodecContext *context, AVCodec *codec)
    {
        Status status = avcodec_open2(context, codec, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot open decoder", status);
        }
    }
};

/**
 * @brief Decode an image (AVPacket -> AVFrame). Require a decoder context.
 */
class ImageDecoder
{
public:
    ImageDecoder(AVCodecContext *context)
        : _context(context)
    {
    }

    void sendPacket(AVPacket *packet)
    {
        Status status = avcodec_send_packet(_context, packet);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot send input packet", status);
        }
    }

    void flush()
    {
        Status status = avcodec_send_packet(_context, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot flush input decoder", status);
        }
    }

    void receiveFrame(AVFrame *frame)
    {
        Status status = avcodec_receive_frame(_context, frame);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot receive decoded frame", status);
        }
    }

private:
    AVCodecContext *_context;
};

/**
 * @brief Combine all operations to read an image from a file and get a frame.
 * - Create the input context to read the file
 * - Find the codec info in the input context video stream.
 * - Create the decoder context using this codec.
 * - Read the raw frame (Packet)
 * - Decode the raw frame to get the final frame
 */
class ImageReader
{
public:
    static FramePtr read(const std::string &filename)
    {
        auto context = InputContext::create(filename);
        auto codec = _createCodec(context.get());
        auto packet = _readRawImage(context.get());
        auto frame = _decodeImage(codec.get(), packet.get());
        return frame;
    }

private:
    static CodecContextPtr _createCodec(AVFormatContext *context)
    {
        auto codec = InputVideoStream::findCodec(context);
        return DecoderContext::create(codec);
    }

    static PacketPtr _readRawImage(AVFormatContext *context)
    {
        auto packet = Packet::create();
        _readPacket(context, packet.get());
        return packet;
    }

    static void _readPacket(AVFormatContext *context, AVPacket *packet)
    {
        Status status = av_read_frame(context, packet);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot read input packet", status);
        }
    }

    static FramePtr _decodeImage(AVCodecContext *codec, AVPacket *packet)
    {
        ImageDecoder decoder(codec);
        decoder.sendPacket(packet);
        decoder.flush();
        auto frame = Frame::create();
        decoder.receiveFrame(frame.get());
        return frame;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Convert images to desired video format
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Information about the format of an image.
 */
struct ImageFormat
{
    int width = 0;
    int height = 0;
    AVPixelFormat format = AV_PIX_FMT_NONE;

    static ImageFormat fromFrame(const AVFrame *frame)
    {
        ImageFormat info;
        info.width = frame->width;
        info.height = frame->height;
        info.format = AVPixelFormat(frame->format);
        return info;
    }

    void toFrame(AVFrame *frame) const
    {
        frame->width = width;
        frame->height = height;
        frame->format = format;
    }
};

/**
 * @brief Context used to convert an image to another format.
 */
class ConversionContext
{
public:
    struct Deleter
    {
        void operator()(SwsContext *context) const
        {
            sws_freeContext(context);
        }
    };

    using Ptr = std::unique_ptr<SwsContext, Deleter>;

    static Ptr create(const ImageFormat &source, const ImageFormat &destination)
    {
        auto context = sws_getContext(
            source.width,
            source.height,
            source.format,
            destination.width,
            destination.height,
            destination.format,
            SWS_BILINEAR,
            nullptr,
            nullptr,
            nullptr);
        if (!context)
        {
            throw FfmpegException("Cannot allocate conversion context");
        }
        return Ptr(context);
    }
};

using ConversionContextPtr = ConversionContext::Ptr;

/**
 * @brief Convert a given frame to a given format.
 */
class ImageConverter
{
public:
    static FramePtr convert(const AVFrame *frame, const ImageFormat &format)
    {
        auto context = _createContext(frame, format);
        auto newFrame = Frame::create();
        _setupNewFrame(newFrame.get(), format);
        _convert(context.get(), frame, newFrame.get());
        return newFrame;
    }

private:
    static ConversionContextPtr _createContext(const AVFrame *frame, const ImageFormat &format)
    {
        auto sourceFormat = ImageFormat::fromFrame(frame);
        return ConversionContext::create(sourceFormat, format);
    }

    static void _setupNewFrame(AVFrame *frame, const ImageFormat &format)
    {
        format.toFrame(frame);
        Status status = av_frame_get_buffer(frame, 0);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot setup new frame", status);
        }
    }

    static void _convert(SwsContext *context, const AVFrame *source, AVFrame *destination)
    {
        sws_scale(context, source->data, source->linesize, 0, source->height, destination->data, destination->linesize);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Create a video from formatted images
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Information about the format of a video.
 */
struct VideoInfo
{
    AVCodec *codec = nullptr;
    int width = 0;
    int height = 0;
    AVPixelFormat format = AV_PIX_FMT_NONE;
    int64_t bitrate = 0;
    int framerate = 0;

    static AVCodec *getCodec(const std::string &name)
    {
        auto codec = avcodec_find_encoder_by_name(name.c_str());
        if (!codec)
        {
            throw FfmpegException("Invalid codec '" + name + "'");
        }
        return codec;
    }

    static AVPixelFormat getFormat(const std::string &name)
    {
        auto format = av_get_pix_fmt(name.c_str());
        if (format == AV_PIX_FMT_NONE)
        {
            throw FfmpegException("Invalid pixel format: '" + name + "'");
        }
        return format;
    }
};

/**
 * @brief Create an output context to write a video file.
 */
class OutputContext
{
public:
    static FormatContextPtr create(const std::string &filename)
    {
        AVFormatContext *context = nullptr;
        Status status = avformat_alloc_output_context2(&context, nullptr, nullptr, filename.c_str());
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot allocate output context", status);
        }
        return FormatContextPtr(context);
    }
};

/**
 * @brief Add an output video stream to an output context.
 */
class OutputVideoStream
{
public:
    static AVStream *add(AVFormatContext *context, const VideoInfo &info)
    {
        auto stream = _addStream(context, info.codec);
        _setupParameters(stream, info);
        return stream;
    }

private:
    static AVStream *_addStream(AVFormatContext *context, AVCodec *codec)
    {
        auto stream = avformat_new_stream(context, codec);
        if (!stream)
        {
            throw FfmpegException("Cannot create output video stream");
        }
        return stream;
    }

    static void _setupParameters(AVStream *stream, const VideoInfo &info)
    {
        stream->codecpar->codec_id = info.codec->id;
        stream->codecpar->codec_type = info.codec->type;
        stream->codecpar->width = info.width;
        stream->codecpar->height = info.height;
        stream->codecpar->format = info.format;
        stream->codecpar->bit_rate = info.bitrate;
        stream->time_base = {1, info.framerate};
    }
};

/**
 * @brief Information required for encoding.
 */
struct EncoderInfo
{
    AVFormatContext *context = nullptr;
    AVCodec *codec = nullptr;
    AVStream *stream = nullptr;
    int framerate = 0;
};

/**
 * @brief Create the context to encode frames.
 */
class EncoderContext
{
public:
    static CodecContextPtr create(const EncoderInfo &info)
    {
        auto context = CodecContext::create(info.codec);
        _setup(context.get(), info.stream->codecpar);
        _loadInfo(context.get(), info);
        _openContext(context.get(), info.codec);
        _updateStream(context.get(), info.stream);
        return context;
    }

private:
    static void _setup(AVCodecContext *context, AVCodecParameters *parameters)
    {
        Status status = avcodec_parameters_to_context(context, parameters);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot setup encoder context", status);
        }
    }

    static void _loadInfo(AVCodecContext *context, const EncoderInfo &info)
    {
        context->time_base = {1, info.framerate};
        if (info.stream->codecpar->codec_id == AV_CODEC_ID_H264)
        {
            av_opt_set(context->priv_data, "preset", "ultrafast", 0);
        }
        if (info.context->oformat->flags & AVFMT_GLOBALHEADER)
        {
            context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    static void _openContext(AVCodecContext *context, AVCodec *codec)
    {
        Status status = avcodec_open2(context, codec, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot open encoder", status);
        }
    }

    static void _updateStream(AVCodecContext *context, AVStream *stream)
    {
        Status status = avcodec_parameters_from_context(stream->codecpar, context);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot setup video stream", status);
        }
    }
};

/**
 * @brief Encode video frames (Frame to Packet). Requires a context.
 */
class VideoEncoder
{
public:
    void open(AVCodecContext *context)
    {
        _context = context;
    }

    void sendFrame(const AVFrame *frame)
    {
        Status status = avcodec_send_frame(_context, frame);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot send output frame", status);
        }
    }

    void flush()
    {
        Status status = avcodec_send_frame(_context, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot flush output decoder", status);
        }
    }

    bool receivePacket(AVPacket *packet)
    {
        Status status = avcodec_receive_packet(_context, packet);
        if (status.isRetry())
        {
            return false;
        }
        if (status.isError())
        {
            throw FfmpegException("Cannot receive output packet", status);
        }
        return true;
    }

private:
    AVCodecContext *_context = nullptr;
};

/**
 * @brief Write encoded frames (Packets) into a video file (output context).
 */
class VideoWriter
{
public:
    void open(AVFormatContext *context)
    {
        _context = context;
        _openVideo();
        _writeHeader();
    }

    void write(AVPacket *packet)
    {
        Status status = av_interleaved_write_frame(_context, packet);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot write video frame", status);
        }
    }

    void close()
    {
        _writeTrailer();
        _closeVideo();
    }

private:
    bool _isFileOpenedByDemuxer() const
    {
        return _context->oformat->flags & AVFMT_NOFILE;
    }

    void _openVideo()
    {
        if (_isFileOpenedByDemuxer())
        {
            return;
        }
        Status status = avio_open(&_context->pb, _context->url, AVIO_FLAG_WRITE);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot open video IO context", status);
        }
    }

    void _writeHeader()
    {
        Status status = avformat_write_header(_context, nullptr);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot write video header", status);
        }
    }

    void _writeTrailer()
    {
        Status status = av_write_trailer(_context);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot write video trailer", status);
        }
    }

    void _closeVideo()
    {
        if (_isFileOpenedByDemuxer())
        {
            return;
        }
        if (!_context->pb || !_context->pb->write_flag)
        {
            return;
        }
        Status status = avio_closep(&_context->pb);
        if (!status.isSuccess())
        {
            throw FfmpegException("Cannot close video", status);
        }
    }

    AVFormatContext *_context = nullptr;
};

/**
 * @brief Manage the timestamps of the frames written to a video file.
 */
class VideoTimer
{
public:
    void setFramerate(int framerate)
    {
        _framerate = framerate;
    }

    void setTimebase(AVRational timebase)
    {
        _frameDuration = timebase.den / (_framerate * timebase.num);
    }

    void setupPacket(AVPacket *packet)
    {
        packet->pts = getTimestamp();
        packet->dts = packet->pts;
        packet->duration = _frameDuration;
        ++_frameCount;
    }

    int64_t getTimestamp() const
    {
        return _frameCount * _frameDuration;
    }

private:
    int64_t _framerate = 0;
    int64_t _frameCount = 0;
    int64_t _frameDuration = 0;
};

/**
 * @brief Video stream that encode and write frames to a file.
 * - Holds video parameters
 * - Create the output context
 * - Add a video stream to this context
 * - Create the encoder
 * - Open a video writer using the context
 * - Encode frames before writing them in the output context.
 */
class VideoStream
{
public:
    VideoStream(const VideoInfo &info)
        : _info(info)
    {
        _timer.setFramerate(_info.framerate);
    }

    void open(const std::string &filename)
    {
        _context = OutputContext::create(filename);
        _stream = OutputVideoStream::add(_context.get(), _info);
        _encoderContext = _createEncoder();
        _encoder.open(_encoderContext.get());
        _writer.open(_context.get());
        _timer.setTimebase(_stream->time_base);
    }

    void write(AVFrame *frame)
    {
        _encoder.sendFrame(frame);
        auto packet = Packet::create();
        if (!_encoder.receivePacket(packet.get()))
        {
            return;
        }
        _writePacket(packet.get());
    }

    void close()
    {
        _flushEncoder();
        _writer.close();
    }

private:
    CodecContextPtr _createEncoder()
    {
        EncoderInfo encoderInfo;
        encoderInfo.context = _context.get();
        encoderInfo.codec = _info.codec;
        encoderInfo.stream = _stream;
        encoderInfo.framerate = _info.framerate;
        return EncoderContext::create(encoderInfo);
    }

    void _writePacket(AVPacket *packet)
    {
        packet->stream_index = _stream->index;
        _timer.setupPacket(packet);
        _writer.write(packet);
    }

    void _flushEncoder()
    {
        _encoder.flush();
        auto packet = Packet::create();
        while (_encoder.receivePacket(packet.get()))
        {
            _writePacket(packet.get());
            packet = Packet::create();
        }
    }

    VideoInfo _info;
    FormatContextPtr _context;
    VideoWriter _writer;
    AVStream *_stream;
    CodecContextPtr _encoderContext;
    VideoEncoder _encoder;
    VideoTimer _timer;
};

/**
 * @brief Wrapper around a video stream to write frames from their filename
 * directly and add context information to the exceptions.
 */
class Movie
{
public:
    Movie(const VideoInfo &info)
        : _stream(info)
    {
        _conversionInfo.width = info.width;
        _conversionInfo.height = info.height;
        _conversionInfo.format = info.format;
    }

    Movie(const Movie &) = delete;
    Movie &operator=(const Movie &) = delete;

    void open(const std::string &filename)
    {
        try
        {
            _stream.open(filename);
        }
        catch (const FfmpegException &e)
        {
            std::ostringstream message;
            message << "Error while opening output file '" << filename << "': " << e.getMessage();
            throw FfmpegException(message.str(), e.getStatus());
        }
    }

    void addFrame(const std::string &filename)
    {
        try
        {
            auto image = ImageReader::read(filename);
            auto frame = ImageConverter::convert(image.get(), _conversionInfo);
            _stream.write(frame.get());
        }
        catch (const FfmpegException &e)
        {
            std::ostringstream message;
            message << "Error while processing frame '" << filename << "': " << e.getMessage();
            throw FfmpegException(message.str(), e.getStatus());
        }
    }

    void close()
    {
        try
        {
            _stream.close();
        }
        catch (const FfmpegException &e)
        {
            std::ostringstream message;
            message << "Error while closing output file: " << e.getMessage();
            throw FfmpegException(message.str(), e.getStatus());
        }
    }

private:
    ImageFormat _conversionInfo;
    VideoStream _stream;
};

/**
 * @brief Adapter to convert MovieInfo to FFmpeg video info.
 */
class VideoInfoAdapter
{
public:
    static VideoInfo createVideoInfo(const MovieInfo &movie)
    {
        VideoInfo info;
        info.codec = getCodec(movie);
        info.width = movie.width;
        info.height = movie.height;
        info.format = getFormat(movie);
        info.bitrate = getBitrate(movie);
        info.framerate = movie.framerate;
        return info;
    }

private:
    static int64_t getBitrate(const MovieInfo &info)
    {
        return info.bitrate <= 0 ? computeDefaultBitrate(info) : info.bitrate;
    }

    static int64_t computeDefaultBitrate(const MovieInfo &info)
    {
        return int64_t(3) * int64_t(info.width) * int64_t(info.height) * int64_t(info.framerate);
    }

    static AVCodec *getCodec(const MovieInfo &info)
    {
        return VideoInfo::getCodec(info.codec.empty() ? "libx264" : info.codec);
    }

    static AVPixelFormat getFormat(const MovieInfo &info)
    {
        return VideoInfo::getFormat(info.format.empty() ? "yuv420p" : info.format);
    }
};

/**
 * @brief FFmpeg implementation of the movie maker
 */
class FfmpegMovieMaker
{
public:
    static void createMovie(const MovieInfo &info)
    {
        try
        {
            _createMovie(info);
        }
        catch (const FfmpegException &e)
        {
            throw MovieCreationException(_formatError(e));
        }
    }

private:
    static void _createMovie(const MovieInfo &info)
    {
        auto videoInfo = VideoInfoAdapter::createVideoInfo(info);
        Movie movie(videoInfo);
        movie.open(info.outputFile);
        for (const auto &frame : info.inputFiles)
        {
            movie.addFrame(frame);
        }
        movie.close();
    }

    static std::string _formatError(const FfmpegException &e)
    {
        std::ostringstream stream;
        stream << "FFmpeg error: '" << e.getMessage() << "' (code: " << e.getStatusCode() << " '"
               << e.getStatusDescription() << "')";
        return stream.str();
    }
};
} // namespace

void MovieMaker::createMovie(const MovieInfo &info)
{
    FfmpegMovieMaker::createMovie(info);
}

#else
void MovieMaker::createMovie(const MovieInfo &)
{
    throw MovieCreationException(
        "This version of Brayns was not compiled with FFmpeg, movie generation "
        "is hence disabled");
}
#endif // BRAYNS_USE_FFMPEG
