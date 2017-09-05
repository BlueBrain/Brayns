/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "LivreEngine.h"

#include "LivreCamera.h"
#include "LivreFrameBuffer.h"
#include "LivreRenderer.h"
#include "LivreScene.h"

#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/parameters/ParametersManager.h>

#include <lexis/render/Histogram.h>
#include <livre/data/DataSource.h>
#include <livre/data/VolumeInformation.h>
#include <livre/eq/Engine.h>
#include <livre/eq/FrameData.h>
#include <livre/lib/configuration/ApplicationParameters.h>
#include <livre/lib/configuration/VolumeRendererParameters.h>

#ifdef BRAYNS_USE_ZEROEQ
#include <plugins/extensions/plugins/ZeroEQPlugin.h>
#endif

namespace brayns
{
LivreEngine::LivreEngine(int argc, char** argv,
                         ParametersManager& parametersManager)
    : Engine(parametersManager)
{
    // force offscreen rendering
    ::setenv("EQ_WINDOW_IATTR_HINT_DRAWABLE", "-12" /*FBO*/, 1 /*overwrite*/);

    // deflect streaming is handled by Brayns, disable Equalizer-integrated
    // stream
    const char* deflectHostEnv = "DEFLECT_HOST";
    auto deflectHost = ::getenv(deflectHostEnv);
    ::unsetenv(deflectHostEnv);

    // disable all logging
    lunchbox::Log::level = 0;
    lunchbox::Log::topics = 0;

    strings arguments;
    for (int i = 0; i < argc; ++i)
        arguments.push_back(argv[i]);
    const auto& volumeFile =
        _parametersManager.getVolumeParameters().getFilename();
    if (!volumeFile.empty())
    {
        if (livre::DataSource::handles(servus::URI(volumeFile)))
        {
            auto i = std::find(arguments.begin(), arguments.end(), "--volume");
            if (i == arguments.end())
            {
                arguments.push_back("--volume");
                arguments.push_back(volumeFile);
            }
            else
            {
                ++i;
                *i = volumeFile;
            }
        }
    }
    arguments.push_back("--disable-communicator");

    char** newArgv = new char*[arguments.size()];
    for (size_t i = 0; i < arguments.size(); ++i)
        newArgv[i] = const_cast<char*>(arguments[i].c_str());

    _livre.reset(new livre::Engine(arguments.size(), newArgv));
    delete[] newArgv;

    if (deflectHost)
        ::setenv(deflectHostEnv, deflectHost, 1);

    auto& volParams = parametersManager.getVolumeParameters();
    auto& rendererParams = _parametersManager.getRenderingParameters();

    volParams.setFilename(_livre->getApplicationParameters().dataFileName);

    _activeRenderer = parametersManager.getRenderingParameters().getRenderer();

    Renderers renderersForScene;
    RendererPtr renderer(new LivreRenderer(_parametersManager, *_livre));
    for (const auto rendererType : rendererParams.getRenderers())
    {
        _renderers[rendererType] = renderer;
        renderersForScene.push_back(renderer);
    }

    _scene.reset(
        new LivreScene(renderersForScene, _parametersManager, *_livre));
    renderer->setScene(_scene);

    _frameSize = _parametersManager.getApplicationParameters().getWindowSize();
    _frameBuffer.reset(
        new LivreFrameBuffer(_frameSize, FrameBufferFormat::bgra_i8, *_livre));
    _camera.reset(new LivreCamera(rendererParams.getCameraType(), *_livre));
}

LivreEngine::~LivreEngine()
{
}

std::string LivreEngine::name() const
{
    return "livre";
}

void LivreEngine::extensionInit(ExtensionPlugin& plugin LB_UNUSED)
{
#ifdef BRAYNS_USE_ZEROEQ
    ZeroEQPlugin* zeroeqPlugin = dynamic_cast<ZeroEQPlugin*>(&plugin);
    if (!zeroeqPlugin)
        return;

    zeroeqPlugin->handle(_livre->getFrameData().getVRParameters());
#endif
}

void LivreEngine::commit()
{
    Engine::commit();
    _renderers[_activeRenderer]->commit();
}

void LivreEngine::render()
{
    Engine::render();
    _renderers[_activeRenderer]->render(_frameBuffer);
}

void LivreEngine::postRender()
{
    if (!_scene->getVolumeHandler())
        return;

    const auto& livreHistogram = _livre->getHistogram();
    Histogram braynsHistogram;
    braynsHistogram.range.x() = livreHistogram.getMin();
    braynsHistogram.range.y() = livreHistogram.getMax();
    braynsHistogram.values = livreHistogram.getBinsVector();
    _scene->getVolumeHandler()->setHistogram(braynsHistogram);
}

Vector2ui LivreEngine::getMinimumFrameSize() const
{
    return {1, 1};
}
}
