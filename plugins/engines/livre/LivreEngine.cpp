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

#include <livre/core/data/Histogram.h>
#include <livre/core/data/VolumeInformation.h>
#include <livre/eq/Engine.h>
#include <livre/eq/FrameData.h>
#include <livre/lib/configuration/ApplicationParameters.h>
#include <livre/lib/configuration/VolumeRendererParameters.h>

#ifdef BRAYNS_USE_ZEROEQ
#include <plugins/extensions/plugins/ZeroEQPlugin.h>
#endif

namespace brayns
{

LivreEngine::LivreEngine( int argc, char **argv,
                          ParametersManager& parametersManager )
    : Engine( parametersManager )
{
    // force offscreen rendering
    ::setenv( "EQ_WINDOW_IATTR_HINT_DRAWABLE", "-12" /*FBO*/, 1 /*overwrite*/ );

    // disable all logging
    lunchbox::Log::level = 0;
    lunchbox::Log::topics = 0;

    _livre.reset( new livre::Engine( argc, argv ));

    auto& volParams = parametersManager.getVolumeParameters();
    auto& rendererParams = _parametersManager.getRenderingParameters();

    const auto& vrParams = _livre->getFrameData().getVRParameters();
    volParams.setFilename( _livre->getApplicationParameters().dataFileName );
    rendererParams.setSamplesPerPixel( vrParams.getSamplesPerPixel( ));

    _activeRenderer = parametersManager.getRenderingParameters().getRenderer();

    Renderers renderersForScene;
    RendererPtr renderer( new LivreRenderer( _parametersManager, *_livre ));
    for( const auto rendererType: rendererParams.getRenderers( ))
    {
        _renderers[ rendererType ] = renderer;
        renderersForScene.push_back( renderer );
    }

    _scene.reset( new LivreScene( renderersForScene, _parametersManager, *_livre ));
    renderer->setScene( _scene );

    _frameSize = _parametersManager.getApplicationParameters().getWindowSize();
    _frameBuffer.reset( new LivreFrameBuffer( _frameSize, FBF_BGRA_I8, *_livre ));
    _camera.reset( new LivreCamera( rendererParams.getCameraType(), *_livre ));

    Boxf& worldBounds = _scene->getWorldBounds();
    const auto& volInfo = _livre->getVolumeInformation();
    worldBounds.merge( -volInfo.worldSize/2.f );
    worldBounds.merge( volInfo.worldSize/2.f );
}

LivreEngine::~LivreEngine()
{
}

std::string LivreEngine::name() const
{
    return "livre";
}

void LivreEngine::extensionInit( ExtensionPlugin& plugin LB_UNUSED )
{
#ifdef BRAYNS_USE_ZEROEQ
    ZeroEQPlugin* zeroeqPlugin = dynamic_cast< ZeroEQPlugin* >( &plugin );
    if( !zeroeqPlugin )
        return;

    zeroeqPlugin->handle( _livre->getFrameData().getVRParameters( ));
#endif
}

void LivreEngine::commit()
{
    Engine::commit();
    _renderers[ _activeRenderer ]->commit();
}

void LivreEngine::render()
{
    _renderers[ _activeRenderer ]->render( _frameBuffer );
}

void LivreEngine::postRender()
{
    const auto& livreHistogram = _livre->getHistogram();
    Histogram braynsHistogram;
    braynsHistogram.range.x() = livreHistogram.getMin();
    braynsHistogram.range.y() = livreHistogram.getMax();
    braynsHistogram.values = livreHistogram.getBinsVector();
    _scene->getVolumeHandler()->setHistogram( braynsHistogram );
}

}
