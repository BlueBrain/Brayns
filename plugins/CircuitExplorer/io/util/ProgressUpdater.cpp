#include "ProgressUpdater.h"

ProgressUpdater::ProgressUpdater(
        const brayns::LoaderProgress &cb, const float start, const float end, const size_t numItems)
 : _callback(cb)
 , _start(start)
 , _end(end)
 , _current(_start)
 , _chunks((_end - _start) / numItems)
{
}

void ProgressUpdater::update(const std::string &message) noexcept
{
    _callback.updateProgress(message, _current);
    _current += _chunks;
    _current = _current > _end? _end : _current;
}
