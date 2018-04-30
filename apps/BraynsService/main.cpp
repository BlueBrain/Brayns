/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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
#include <brayns/common/Timer.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/log.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/types.h>

#include <uvw.hpp>

#include <thread>

class BraynsService
{
public:
    BraynsService(int argc, const char** argv)
        : _brayns(argc, argv)
        , _renderingDone{_mainLoop->resource<uvw::AsyncHandle>()}
        , _eventRendering{_mainLoop->resource<uvw::IdleHandle>()}
        , _accumRendering{_mainLoop->resource<uvw::IdleHandle>()}
        , _progressUpdate{_mainLoop->resource<uvw::TimerHandle>()}
        , _checkIdleRendering{_mainLoop->resource<uvw::CheckHandle>()}
        , _triggerRendering{_renderLoop->resource<uvw::AsyncHandle>()}
        , _stopRenderThread{_renderLoop->resource<uvw::AsyncHandle>()}
    {
        _checkIdleRendering->start();

        _setupMainThread();
        _setupRenderThread();

        _brayns.loadPlugins();

        // launch first frame; after that, only events will trigger that
        _eventRendering->start();
    }

    void run()
    {
        // Start render & main loop
        std::thread renderThread(
            [& _renderLoop = _renderLoop] { _renderLoop->run(); });
        _mainLoop->run();

        // Finished
        renderThread.join();
    }

private:
    void _setupMainThread()
    {
        // triggered after rendering, send events to rockets
        _renderingDone->on<uvw::AsyncEvent>([& brayns = _brayns](const auto&,
                                                                 auto&) {
            brayns.postRender();
        });

        // events from rockets, trigger rendering
        _brayns.getEngine().triggerRender = [& eventRendering = _eventRendering]
        {
            eventRendering->start();
        };

        // render or data load trigger from events
        _eventRendering->on<uvw::IdleEvent>([&](const auto&, auto&) {
            _eventRendering->stop();
            _accumRendering->stop();
            _timeSinceLastEvent.start();

            // stop event loop(s) and exit application
            if (!_brayns.getEngine().getKeepRunning())
            {
                _brayns.getEngine().triggerRender = [] {};
                _stopRenderThread->send();
                _mainLoop->stop();
                return;
            }

            // data loading
            if (_brayns.getEngine().rebuildScene())
            {
                if (_isLoading)
                    return;

                _isLoading = true;

                _checkIdleRendering->stop();

                // broadcast progress updates every 100ms
                _progressUpdate->start(std::chrono::milliseconds(0),
                                       std::chrono::milliseconds(100));

                // async load execution
                auto work = _mainLoop->resource<uvw::WorkReq>(
                    [&] { _brayns.buildScene(); });

                // async load finished, restore everything to continue rendering
                work->template on<uvw::WorkEvent>([&](const auto&, auto&) {
                    _brayns.getEngine().markRebuildScene(false);
                    _progressUpdate->stop();
                    _progressUpdate->close();
                    _isLoading = false;

                    _checkIdleRendering->start();
                    _eventRendering->start();
                });

                work->queue();
                return;
            }

            // rendering
            if (_brayns.preRender())
                _triggerRendering->send();
        });

        // send progress updates while we are loading
        _progressUpdate->on<uvw::TimerEvent>([& brayns = _brayns](const auto&,
                                                                  auto&) {
            brayns.postRender();
        });

        // send final progress update, once loading is finished
        _progressUpdate->on<uvw::CloseEvent>([& brayns = _brayns](const auto&,
                                                                  auto&) {
            brayns.postRender();
        });

        // start accum rendering when we have no more other events
        _checkIdleRendering->on<uvw::CheckEvent>([& accumRendering =
                                                      _accumRendering](
            const auto&, auto&) { accumRendering->start(); });

        // accumulation rendering on idle; re-triggered by _checkIdleRendering
        _accumRendering->on<uvw::IdleEvent>([&](const auto&, auto&) {
            if (_timeSinceLastEvent.elapsed() < _idleRenderingDelay)
                return;

            if (_brayns.preRender() && _brayns.getEngine().continueRendering())
                _triggerRendering->send();

            _accumRendering->stop();
        });
    }

    void _setupRenderThread()
    {
        // rendering, triggered from main thread
        _triggerRendering->on<uvw::AsyncEvent>([&](const auto&, auto&) {
            _brayns.renderOnly();
            _renderingDone->send();
        });

        // stop render loop, triggered from main thread
        _stopRenderThread->once<uvw::AsyncEvent>([& renderLoop = _renderLoop](
            const auto&, auto&) { renderLoop->stop(); });
    }

    brayns::Brayns _brayns;

    std::shared_ptr<uvw::Loop> _mainLoop{uvw::Loop::getDefault()};
    std::shared_ptr<uvw::AsyncHandle> _renderingDone;
    std::shared_ptr<uvw::IdleHandle> _eventRendering;
    std::shared_ptr<uvw::IdleHandle> _accumRendering;
    std::shared_ptr<uvw::TimerHandle> _progressUpdate;
    std::shared_ptr<uvw::CheckHandle> _checkIdleRendering;

    std::shared_ptr<uvw::Loop> _renderLoop{uvw::Loop::create()};
    std::shared_ptr<uvw::AsyncHandle> _triggerRendering;
    std::shared_ptr<uvw::AsyncHandle> _stopRenderThread;

    const float _idleRenderingDelay{0.1f};
    bool _isLoading{false};
    brayns::Timer _timeSinceLastEvent;
};

int main(int argc, const char** argv)
{
    try
    {
        BRAYNS_INFO << "Initializing Service..." << std::endl;

        brayns::Timer timer;
        timer.start();

        BraynsService service(argc, argv);

        service.run();

        timer.stop();
        BRAYNS_INFO << "Service was running for " << timer.seconds()
                    << " seconds" << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return 1;
    }
    return 0;
}
