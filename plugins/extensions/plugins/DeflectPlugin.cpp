/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "DeflectPlugin.h"

#include <brayns/Brayns.h>
#include <brayns/common/camera/AbstractManipulator.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/parameters/ApplicationParameters.h>

#include <deflect/SizeHints.h>

#if (BRAYNS_USE_NETWORKING)
#include "ZeroEQPlugin.h"
#endif

namespace
{
const float wheelFactor = 1.f / 40.f;

template <typename T>
std::future<T> make_ready_future(const T value)
{
    std::promise<T> promise;
    promise.set_value(value);
    return promise.get_future();
}
}

namespace brayns
{
#if (BRAYNS_USE_NETWORKING)
DeflectPlugin::DeflectPlugin(ZeroEQPlugin& zeroeq)
#else
DeflectPlugin::DeflectPlugin()
#endif
    : ExtensionPlugin()
    , _sendFuture(make_ready_future(true))
{
    _params.setEnabled(true); // Streaming will only be activated if
                              // the DEFLECT_HOST environment variable
                              // is defined
#if (BRAYNS_USE_NETWORKING)
    if (!zeroeq)
        return;

    zeroeq->handleGET(_params);
    zeroeq->handlePUT("lexis/render/stream-to", _params);
#endif
}

bool DeflectPlugin::run(EnginePtr engine, KeyboardHandler& keyboardHandler,
                        AbstractManipulator& cameraManipulator)
{
    auto& appParams = engine->getParametersManager().getApplicationParameters();
    appParams.setStreamingEnabled(_params.getEnabled());
    appParams.setStreamCompression(_params.getCompression());
    appParams.setStreamQuality(_params.getQuality());

    if (_stream)
    {
        const bool changed = _stream->getId() != _params.getIdString() ||
                             _stream->getHost() != _params.getHostString();
        if (changed)
            _stream.reset();
    }

    if (_previousHost != _params.getHostString())
    {
        _params.setEnabled(true);
        _previousHost = _params.getHostString();
    }

    const bool deflectEnabled = _params.getEnabled();
    if (_stream && _stream->isConnected() && !deflectEnabled)
    {
        BRAYNS_INFO << "Closing Deflect stream" << std::endl;
        _stream.reset();
    }

    const bool observerOnly = engine->haveDeflectPixelOp();
    if (deflectEnabled && !_stream)
    {
        if (_initializeDeflect(observerOnly))
        {
            const auto& windowSize =
                engine->getSupportedFrameSize(appParams.getWindowSize());
            deflect::SizeHints sizeHints;
            sizeHints.preferredWidth = windowSize.x();
            sizeHints.preferredHeight = windowSize.y();
            const auto& minSize = engine->getMinimumFrameSize();
            sizeHints.minWidth = minSize.x();
            sizeHints.minHeight = minSize.y();
            _stream->sendSizeHints(sizeHints);
        }
    }

    if (deflectEnabled && _stream && _stream->isConnected())
    {
        if (!observerOnly)
            _sendDeflectFrame(*engine);

        if (_handleDeflectEvents(*engine, keyboardHandler, cameraManipulator))
        {
            engine->getFrameBuffer().clear();
            engine->getRenderer().commit();
        }
    }

    return true;
}

bool DeflectPlugin::_initializeDeflect(const bool observerOnly)
{
    try
    {
        if (observerOnly)
        {
            _stream.reset(new deflect::Observer(_params.getIdString(),
                                                _params.getHostString(),
                                                _params.getPort()));
        }
        else
            _stream.reset(new deflect::Stream(_params.getIdString(),
                                              _params.getHostString(),
                                              _params.getPort()));

        if (_stream->isConnected())
            BRAYNS_INFO << "Deflect successfully connected to Tide on host "
                        << _stream->getHost() << std::endl;
        else
            BRAYNS_ERROR << "Deflect failed to connect to Tide on host "
                         << _stream->getHost() << std::endl;

        if (!_stream->registerForEvents())
            BRAYNS_ERROR << "Deflect failed to register for events!"
                         << std::endl;

        _params.setId(_stream->getId());
        _params.setHost(_stream->getHost());
        return true;
    }
    catch (std::runtime_error& ex)
    {
        BRAYNS_ERROR << "Deflect failed to initialize. " << ex.what()
                     << std::endl;
        _params.setEnabled(false);
        return false;
    }
}

void DeflectPlugin::_sendDeflectFrame(Engine& engine)
{
    if (!_sendFuture.get())
    {
        if (!_stream->isConnected())
            BRAYNS_INFO << "Stream closed, exiting." << std::endl;
        else
        {
            BRAYNS_ERROR << "failure in _sendDeflectFrame()" << std::endl;
            _params.setEnabled(false);
        }

        return;
    }

    auto& frameBuffer = engine.getFrameBuffer();
    const Vector2i& frameSize = frameBuffer.getSize();
    void* data = frameBuffer.getColorBuffer();

    if (data)
    {
        const size_t bufferSize =
            frameSize.x() * frameSize.y() * frameBuffer.getColorDepth();
        _lastImage.data.resize(bufferSize);
        memcpy(_lastImage.data.data(), data, bufferSize);
        _lastImage.size = frameSize;
        _lastImage.format = frameBuffer.getFrameBufferFormat();

        _send(engine, true);
    }
    else
        _sendFuture = make_ready_future(true);
}

bool DeflectPlugin::_handleDeflectEvents(Engine& engine,
                                         KeyboardHandler& keyboardHandler,
                                         AbstractManipulator& cameraManipulator)
{
    if (!_stream->hasEvent())
        return false;

    while (_stream->hasEvent())
    {
        const deflect::Event& event = _stream->getEvent();
        switch (event.type)
        {
        case deflect::Event::EVT_PRESS:
            _previousPos =
                _getWindowPos(event, engine.getFrameBuffer().getSize());
            _pan = _pinch = false;
            break;
        case deflect::Event::EVT_MOVE:
        case deflect::Event::EVT_RELEASE:
        {
            const auto pos =
                _getWindowPos(event, engine.getFrameBuffer().getSize());
            if (!_pan && !_pinch)
                cameraManipulator.dragLeft(pos, _previousPos);
            _previousPos = pos;
            _pan = _pinch = false;
            break;
        }
        case deflect::Event::EVT_PAN:
        {
            if (_pinch)
                break;
            const auto pos =
                _getWindowPos(event, engine.getFrameBuffer().getSize());
            cameraManipulator.dragMiddle(pos, _previousPos);
            _previousPos = pos;
            _pan = true;
            break;
        }
        case deflect::Event::EVT_PINCH:
        {
            if (_pan)
                break;
            const auto pos =
                _getWindowPos(event, engine.getFrameBuffer().getSize());
            const auto delta =
                _getZoomDelta(event, engine.getFrameBuffer().getSize());
            cameraManipulator.wheel(pos, delta * wheelFactor);
            _pinch = true;
            break;
        }
        case deflect::Event::EVT_KEY_PRESS:
        {
            keyboardHandler.handleKeyboardShortcut(event.text[0]);
            break;
        }
        case deflect::Event::EVT_VIEW_SIZE_CHANGED:
        {
            Vector2ui newSize(event.dx, event.dy);
            if (engine.getCamera().getType() == CameraType::stereo)
                newSize.x() *= 2;

            engine.getParametersManager()
                .getApplicationParameters()
                .setWindowSize(engine.getSupportedFrameSize(newSize));
            break;
        }
        case deflect::Event::EVT_CLOSE:
            _params.setEnabled(false);
            _params.setHost("");
            _previousHost.clear();
            _stream.reset();
            _sendFuture = make_ready_future(true);
            return true;
        default:
            break;
        }
    }

    return true;
}

void DeflectPlugin::_send(const Engine& engine, const bool swapYAxis)
{
    deflect::PixelFormat format = deflect::RGBA;
    switch (_lastImage.format)
    {
    case FrameBufferFormat::bgra_i8:
        format = deflect::BGRA;
        break;
    case FrameBufferFormat::rgb_i8:
        format = deflect::RGB;
        break;
    default:
        format = deflect::RGBA;
    }

    deflect::ImageWrapper deflectImage(_lastImage.data.data(),
                                       _lastImage.size.x(), _lastImage.size.y(),
                                       format);
    if (engine.getCamera().getType() == CameraType::stereo)
        deflectImage.view = deflect::View::side_by_side;

    deflectImage.compressionQuality = _params.getQuality();
    deflectImage.compressionPolicy = _params.getCompression()
                                         ? deflect::COMPRESSION_ON
                                         : deflect::COMPRESSION_OFF;
    if (swapYAxis)
        deflect::ImageWrapper::swapYAxis((void*)_lastImage.data.data(),
                                         _lastImage.size.x(),
                                         _lastImage.size.y(), 4);

    _sendFuture =
        static_cast<deflect::Stream&>(*_stream).asyncSend(deflectImage);
}

Vector2d DeflectPlugin::_getWindowPos(const deflect::Event& event,
                                      const Vector2ui& windowSize) const
{
    return {event.mouseX * windowSize.x(), event.mouseY * windowSize.y()};
}

double DeflectPlugin::_getZoomDelta(const deflect::Event& pinchEvent,
                                    const Vector2ui& windowSize) const
{
    const auto dx = pinchEvent.dx * windowSize.x();
    const auto dy = pinchEvent.dy * windowSize.y();
    return std::copysign(std::sqrt(dx * dx + dy * dy), dx + dy);
}
}
