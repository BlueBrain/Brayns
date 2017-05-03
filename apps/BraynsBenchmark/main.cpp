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
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/types.h>

//#include "ittnotify.h"

int main(int argc, const char** argv)
{
//    __itt_pause();
    try
    {
        const size_t nbFrames = 100;

        std::chrono::high_resolution_clock::time_point startTime;
        uint64_t duration;
        startTime = std::chrono::high_resolution_clock::now();

        brayns::Brayns brayns(argc, argv);

        duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::high_resolution_clock::now() - startTime)
                       .count();
        BRAYNS_INFO << "[PERF] Scene initialization took " << duration
                    << " milliseconds" << std::endl;

        auto& engine = brayns.getEngine();
        auto& scene = engine.getScene();
        auto& bounds = scene.getWorldBounds();
        const float radius = bounds.getSize().find_max();
        startTime = std::chrono::high_resolution_clock::now();
//        __itt_resume(); 
        for (size_t frame = 0; frame < nbFrames; ++frame)
        {
            const brayns::Vector3f target = bounds.getCenter();
            const brayns::Vector3f origin =
                target + brayns::Vector3f(radius * cos(frame * M_PI / 180.f),
                                          0.f,
                                          radius * sin(frame * M_PI / 180.f));
            const brayns::Vector3f up = {0.f, 1.f, 0.f};
            engine.getCamera().set(origin, target, up);

            brayns.render();
        }
//        __itt_pause();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::high_resolution_clock::now() - startTime)
                       .count();
        BRAYNS_INFO << "[PERF] Rendering " << nbFrames << " frames took "
                    << duration << " milliseconds" << std::endl;
        BRAYNS_INFO << "[PERF] Frames per second: "
                    << nbFrames / (duration / 1000.f) << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return 1;
    }
    return 0;
}
