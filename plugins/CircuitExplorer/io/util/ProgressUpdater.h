#pragma once

#include <brayns/io/Loader.h>

class ProgressUpdater
{
public:
    ProgressUpdater(const brayns::LoaderProgress &cb, const float start, const float end, const size_t items);

    void update(const std::string &message) noexcept;
private:
    const brayns::LoaderProgress &_callback;
    const float _start {};
    const float _end {};
    float _current;
    float _chunks;
};
