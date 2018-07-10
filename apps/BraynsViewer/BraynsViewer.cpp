/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#include "BraynsViewer.h"

#include <brayns/Brayns.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/parameters/ParametersManager.h>

#include <thread>

namespace brayns
{
BraynsViewer::BraynsViewer(Brayns& brayns)
    : BaseWindow(brayns)
{
    brayns.loadPlugins();
}

void BraynsViewer::display()
{
    const auto& ap = _brayns.getParametersManager().getApplicationParameters();
    std::stringstream ss;
    ss << "Brayns Viewer [" << ap.getEngineAsString(ap.getEngine()) << "] ";
    auto animationFrame =
        _brayns.getParametersManager().getAnimationParameters().getFrame();
    if (animationFrame != std::numeric_limits<uint32_t>::max())
        ss << " (frame " << animationFrame << ")";
    if (_brayns.getParametersManager()
            .getApplicationParameters()
            .isBenchmarking())
    {
        ss << " @ " << _timer.perSecondSmoothed() << " FPS";
    }
    setTitle(ss.str());

    BaseWindow::display();

    if (_brayns.getEngine().rebuildScene())
    {
        _brayns.getEngine().markRebuildScene(false);
        std::thread([& brayns = _brayns]() { brayns.buildScene(); }).detach();
    }
}
}
