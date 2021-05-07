#pragma once

#include <string>

extern "C"
{
#include <libavutil/common.h>
#include <libavutil/error.h>
}

namespace ffmpeg
{
class Status
{
public:
    Status() = default;

    Status(int code)
        : _code(code)
    {
    }

    int getCode() const { return _code; }

    bool isError() const { return _code < 0; }

    bool isSuccess() const { return _code == 0; }

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

    operator int() const { return _code; }

private:
    int _code = 0;
};
} // namespace ffmpeg