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
#include <brayns/common/log.h>
#include <brayns/common/types.h>
#include <brayns/engineapi/Camera.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/engineapi/Scene.h>

int main(int argc, const char** argv)
{
    try
    {
        const size_t nbFrames = 100;

        brayns::Timer timer;

        timer.start();
        brayns::Brayns brayns(argc, argv);
        timer.stop();

        BRAYNS_INFO << "[PERF] Scene initialization took "
                    << timer.milliseconds() << " milliseconds" << std::endl;

        auto& engine = brayns.getEngine();
        auto& scene = engine.getScene();
        const auto bounds = scene.getBounds();
        const double radius = glm::compMax(bounds.getSize());
        timer.start();
        for (size_t frame = 0; frame < nbFrames; ++frame)
        {
            const brayns::Vector3d& center = bounds.getCenter();
            const auto quat = glm::angleAxis(frame * M_PI / 180.0,
                                             brayns::Vector3d(0.0, 1.0, 0.0));
            const brayns::Vector3d dir =
                glm::rotate(quat, brayns::Vector3d(0, 0, -1));
            engine.getCamera().set(center + radius * -dir, quat);
            brayns.commitAndRender();
        }
        timer.stop();

        BRAYNS_INFO << "[PERF] Rendering " << nbFrames << " frames took "
                    << timer.milliseconds() << " milliseconds" << std::endl;
        BRAYNS_INFO << "[PERF] Frames per second: "
                    << nbFrames / timer.seconds() << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return 1;
    }
    return 0;
}
