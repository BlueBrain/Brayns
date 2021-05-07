#pragma once

#include <string>

extern "C"
{
#include <libavformat/avformat.h>
}

#include "../Exception.h"
#include "../Status.h"

namespace ffmpeg
{
class VideoWriter
{
public:
    void open(AVFormatContext* context)
    {
        _context = context;
        _openVideo();
        _writeHeader();
    }

    void write(AVPacket* packet)
    {
        Status status = av_interleaved_write_frame(_context, packet);
        if (!status.isSuccess())
        {
            throw Exception("Cannot write video frame", status);
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
        Status status =
            avio_open(&_context->pb, _context->url, AVIO_FLAG_WRITE);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open video IO context", status);
        }
    }

    void _writeHeader()
    {
        Status status = avformat_write_header(_context, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot write video header", status);
        }
    }

    void _writeTrailer()
    {
        Status status = av_write_trailer(_context);
        if (!status.isSuccess())
        {
            throw Exception("Cannot write video trailer", status);
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
            throw Exception("Cannot close video", status);
        }
    }

    AVFormatContext* _context = nullptr;
};
} // namespace ffmpeg