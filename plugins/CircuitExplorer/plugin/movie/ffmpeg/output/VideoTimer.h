#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

namespace ffmpeg
{
class VideoTimer
{
public:
    void setFramerate(int framerate) { _framerate = framerate; }

    void setTimebase(AVRational timebase)
    {
        _frameDuration = timebase.den / (_framerate * timebase.num);
    }

    void setupPacket(AVPacket* packet)
    {
        packet->pts = getTimestamp();
        packet->dts = packet->pts;
        packet->duration = _frameDuration;
        ++_frameCount;
    }

    int64_t getTimestamp() const { return _frameCount * _frameDuration; }

private:
    int64_t _framerate = 0;
    int64_t _frameCount = 0;
    int64_t _frameDuration = 0;
};
} // namespace ffmpeg