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

int main(int argc, const char** argv)
{
    try
    {
        BRAYNS_INFO << "Initializing Service..." << std::endl;

        brayns::Timer timer;
        timer.start();

        brayns::Brayns brayns(argc, argv);

        auto mainLoop = uvw::Loop::getDefault();
        auto renderingDone = mainLoop->resource<uvw::AsyncHandle>();
        auto eventRendering = mainLoop->resource<uvw::IdleHandle>();
        auto accumRendering = mainLoop->resource<uvw::IdleHandle>();
        auto progressUpdate = mainLoop->resource<uvw::TimerHandle>();
        auto checkIdleRendering = mainLoop->resource<uvw::CheckHandle>();
        checkIdleRendering->start();

        auto renderLoop = uvw::Loop::create();
        auto triggerRendering = renderLoop->resource<uvw::AsyncHandle>();
        auto stopRenderThread = renderLoop->resource<uvw::AsyncHandle>();

        // main thread
        const float idleRenderingDelay = 0.1f;
        bool isLoading = false;
        brayns::Timer timeSinceLastEvent;
        {
            // triggered after rendering, send events to rockets
            renderingDone->on<uvw::AsyncEvent>(
                [&](const auto&, auto&) { brayns.postRender(); });

            // events from rockets
            brayns.getEngine().triggerRender = [&] { eventRendering->start(); };

            // render or data load trigger from events
            eventRendering->on<uvw::IdleEvent>([&](const auto&, auto&) {
                eventRendering->stop();
                accumRendering->stop();
                timeSinceLastEvent.start();

                // stop event loop(s) and exit application
                if (!brayns.getEngine().getKeepRunning())
                {
                    stopRenderThread->send();
                    mainLoop->stop();
                    return;
                }

                // data loading
                if (brayns.getEngine().rebuildScene())
                {
                    if (isLoading)
                        return;

                    isLoading = true;

                    checkIdleRendering->stop();

                    // broadcast progress updates every 100ms
                    progressUpdate->start(std::chrono::milliseconds(0),
                                          std::chrono::milliseconds(100));

                    // async load execution
                    auto work = mainLoop->resource<uvw::WorkReq>(
                        [&] { brayns.buildScene(); });

                    // async load finished, restore everything to continue
                    // rendering
                    work->template on<uvw::WorkEvent>([&](const auto&, auto&) {
                        brayns.getEngine().markRebuildScene(false);
                        progressUpdate->stop();
                        progressUpdate->close();
                        isLoading = false;

                        checkIdleRendering->start();
                        eventRendering->start();
                    });

                    work->queue();
                    return;
                }

                // rendering
                if (brayns.preRender())
                    triggerRendering->send();
            });

            // send progress updates while we are loading
            progressUpdate->on<uvw::TimerEvent>(
                [&](const auto&, auto&) { brayns.postRender(); });

            // send final progress update, once loading is finished
            progressUpdate->on<uvw::CloseEvent>(
                [&](const auto&, auto&) { brayns.postRender(); });

            // start accum rendering when we have no more other events
            checkIdleRendering->on<uvw::CheckEvent>(
                [&](const auto&, auto&) { accumRendering->start(); });

            // render trigger from going into idle
            accumRendering->on<uvw::IdleEvent>([&](const auto&, auto&) {
                if (timeSinceLastEvent.elapsed() < idleRenderingDelay)
                    return;

                if (brayns.preRender() &&
                    brayns.getEngine().continueRendering())
                    triggerRendering->send();

                accumRendering->stop();
            });
        }

        // render thread
        {
            // rendering
            triggerRendering->on<uvw::AsyncEvent>([&](const auto&, auto&) {
                brayns.renderOnly();
                renderingDone->send();
            });

            // stop render loop
            stopRenderThread->once<uvw::AsyncEvent>(
                [&](const auto&, auto&) { renderLoop->stop(); });
        }

        brayns.createPlugins();

        // Start render & main loop
        std::thread renderThread([&] { renderLoop->run(); });
        mainLoop->run();

        // Finished
        renderThread.join();
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
