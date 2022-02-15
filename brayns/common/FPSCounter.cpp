/*
 * The MIT License (MIT)
 *
 * Copyright © 2021 Nadir Roman Guerrero
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <brayns/common/FPSCounter.h>

namespace brayns
{
void FPSCounter::startFrame() noexcept
{
    _timer.reset();
}

void FPSCounter::endFrame() noexcept
{
    _lastFrameTimeMillis = _timer.millis();
    const auto millisPerSecond = static_cast<double>(std::milli::den);

    const auto frameTimeSeconds = static_cast<double>(_lastFrameTimeMillis) / millisPerSecond;
    const auto currentFPS = 1.0 / frameTimeSeconds;

    _lastFPS = static_cast<uint32_t>(_lastFPS);

    // Popular smooth FPS computation: https://stackoverflow.com/a/87333/2649079
    _smoothFPS = _smoothFPS * 0.9 + currentFPS * 0.1;
}

uint32_t FPSCounter::getAverageFPS() const noexcept
{
    return static_cast<uint32_t>(_smoothFPS);
}

uint32_t FPSCounter::getLastFPS() const noexcept
{
    return _lastFPS;
}

int64_t FPSCounter::getLastFrameTimeMillis() const noexcept
{
    return _lastFrameTimeMillis;
}
}
