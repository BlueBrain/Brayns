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
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/parameters/ParametersManager.h>

namespace
{
const float DEFAULT_TEST_TIMESTAMP = 10000.f;
}

namespace brayns
{
BraynsViewer::BraynsViewer(Brayns& brayns)
    : BaseWindow(brayns)
    , _fieldOfView(45.f)
    , _eyeSeparation(0.0635f)
{
    _registerKeyboardShortcuts();
}

void BraynsViewer::_registerKeyboardShortcuts()
{
    BaseWindow::_registerKeyboardShortcuts();
    auto& keyHandler = _brayns.getKeyboardHandler();
    keyHandler.registerKeyboardShortcut(
        '3', "Set gradient materials",
        std::bind(&BraynsViewer::_gradientMaterials, this));
    keyHandler.registerKeyboardShortcut(
        '4', "Set pastel materials",
        std::bind(&BraynsViewer::_pastelMaterials, this));
    keyHandler.registerKeyboardShortcut(
        '5', "Set random materials",
        std::bind(&BraynsViewer::_randomMaterials, this));
    keyHandler.registerKeyboardShortcut(
        'g', "Enable/Disable timestamp auto-increment",
        std::bind(&BraynsViewer::_toggleIncrementalTimestamp, this));
    keyHandler.registerKeyboardShortcut(
        'x', "Set timestamp to " + std::to_string(DEFAULT_TEST_TIMESTAMP),
        std::bind(&BraynsViewer::_defaultTimestamp, this));
    keyHandler.registerKeyboardShortcut(
        '|', "Create cache file ",
        std::bind(&BraynsViewer::_saveSceneToCacheFile, this));
    keyHandler.registerKeyboardShortcut(
        '{', "Decrease eye separation",
        std::bind(&BraynsViewer::_decreaseEyeSeparation, this));
    keyHandler.registerKeyboardShortcut(
        '}', "Increase eye separation",
        std::bind(&BraynsViewer::_increaseEyeSeparation, this));
    keyHandler.registerKeyboardShortcut(
        '<', "Decrease field of view",
        std::bind(&BraynsViewer::_decreaseFieldOfView, this));
    keyHandler.registerKeyboardShortcut(
        '>', "Increase field of view",
        std::bind(&BraynsViewer::_increaseFieldOfView, this));
}

void BraynsViewer::_decreaseFieldOfView()
{
    _fieldOfView -= 1.f;
    //_fieldOfView = std::max(1.f, _fieldOfView);
    _brayns.getEngine().getCamera().setFieldOfView(_fieldOfView);
    BRAYNS_INFO << "Field of view: " << _fieldOfView << std::endl;
}

void BraynsViewer::_increaseFieldOfView()
{
    _fieldOfView += 1.f;
    //    _fieldOfView = std::min(179.f, _fieldOfView);
    _brayns.getEngine().getCamera().setFieldOfView(_fieldOfView);
    BRAYNS_INFO << "Field of view: " << _fieldOfView << std::endl;
}

void BraynsViewer::_decreaseEyeSeparation()
{
    _eyeSeparation -= 0.01f;
    //_eyeSeparation = std::max(0.1f, _eyeSeparation);
    _brayns.getEngine().getCamera().setEyeSeparation(_eyeSeparation);
    BRAYNS_INFO << "Eye separation: " << _eyeSeparation << std::endl;
}

void BraynsViewer::_increaseEyeSeparation()
{
    _eyeSeparation += 0.01f;
    //_eyeSeparation = std::min(1.0f, _eyeSeparation);
    _brayns.getEngine().getCamera().setEyeSeparation(_eyeSeparation);
    BRAYNS_INFO << "Eye separation: " << _eyeSeparation << std::endl;
}

void BraynsViewer::_gradientMaterials()
{
    _brayns.getEngine().initializeMaterials(MT_GRADIENT);
}

void BraynsViewer::_pastelMaterials()
{
    _brayns.getEngine().initializeMaterials(MT_PASTEL_COLORS);
}

void BraynsViewer::_randomMaterials()
{
    _brayns.getEngine().initializeMaterials(MT_RANDOM);
}

void BraynsViewer::_toggleIncrementalTimestamp()
{
    auto& sceneParams = _brayns.getParametersManager().getSceneParameters();
    sceneParams.setAnimationDelta(sceneParams.getAnimationDelta() == 0 ? 1 : 0);
}

void BraynsViewer::_defaultTimestamp()
{
    auto& sceneParams = _brayns.getParametersManager().getSceneParameters();
    sceneParams.setTimestamp(DEFAULT_TEST_TIMESTAMP);
}

void BraynsViewer::_saveSceneToCacheFile()
{
    auto& scene = _brayns.getEngine().getScene();
    scene.saveSceneToCacheFile();
}

void BraynsViewer::display()
{
    std::stringstream ss;
    ss << "Brayns Viewer ["
       << _brayns.getParametersManager().getRenderingParameters().getEngine()
       << "] ";
    size_t ts =
        _brayns.getParametersManager().getSceneParameters().getTimestamp();
    if (ts != std::numeric_limits<size_t>::max())
        ss << " (frame " << ts << ")";
    if (_brayns.getParametersManager()
            .getApplicationParameters()
            .isBenchmarking())
    {
        ss << " @ " << _fps.getFPS();
    }
    setTitle(ss.str());

    BaseWindow::display();
}
}
