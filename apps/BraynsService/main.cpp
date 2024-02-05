/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <brayns/Brayns.h>
#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>
#include <brayns/engine/Engine.h>
#include <brayns/network/interface/ActionInterface.h>

#include <atomic>

class BraynsService
{
public:
    BraynsService(int argc, const char** argv)
        : _brayns(argc, argv)
    {
        if (!_brayns.getActionInterface())
        {
            throw std::runtime_error("No action interface registered");
        }
        auto& engine = _brayns.getEngine();
        engine.triggerRender = [this] { _triggerRender(); };
    }

    void run()
    {
        auto& engine = _brayns.getEngine();
        auto& interface = *_brayns.getActionInterface();
        while (engine.getKeepRunning())
        {
            interface.processRequests();
            if (_isRenderTriggered() || engine.continueRendering())
            {
                _brayns.commitAndRender();
            }
            interface.update();
        }
    }

private:
    void _triggerRender() { _renderTriggered = true; }

    bool _isRenderTriggered()
    {
        if (_renderTriggered)
        {
            _renderTriggered = false;
            return true;
        }
        return false;
    }

    brayns::Brayns _brayns;
    std::atomic_bool _renderTriggered{false};
};

int main(int argc, const char** argv)
{
    try
    {
        brayns::Timer timer;
        timer.start();

        BraynsService service(argc, argv);
        service.run();

        timer.stop();
        brayns::Log::info("Service was running for {} seconds.",
                          timer.seconds());
    }
    catch (const std::runtime_error& e)
    {
        brayns::Log::error(e.what());
        return 1;
    }
    return 0;
}
