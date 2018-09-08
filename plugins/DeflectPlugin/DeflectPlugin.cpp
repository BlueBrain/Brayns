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
#include <brayns/parameters/ParametersManager.h>

#include <brayns/pluginapi/PluginAPI.h>

#include <deflect/SizeHints.h>
#include <deflect/Stream.h>

#ifdef BRAYNS_USE_LIBUV
#include <uvw.hpp>
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
} // namespace

namespace brayns
{
class DeflectPlugin::Impl
{
public:
    Impl(EnginePtr engine, PluginAPI* api)
        : _engine(engine)
        , _appParams{api->getParametersManager().getApplicationParameters()}
        , _params{api->getParametersManager().getStreamParameters()}
        , _keyboardHandler(api->getKeyboardHandler())
        , _cameraManipulator(api->getCameraManipulator())
        , _sendFuture{make_ready_future(true)}
    {
        // Streaming will only be activated if the DEFLECT_HOST environment
        // variable is defined
        _params.setEnabled(true);
    }

    void preRender()
    {
        if (_stream)
        {
            const bool changed = _stream->getId() != _params.getId() ||
                                 _stream->getHost() != _params.getHostname() ||
                                 _stream->getPort() != _params.getPort();
            if (changed)
                _closeStream();
        }

        if (_previousHost != _params.getHostname())
        {
            _params.setEnabled(true);
            _previousHost = _params.getHostname();
        }

        const bool deflectEnabled = _params.getEnabled();
        if (_stream && _stream->isConnected() && !deflectEnabled)
            _closeStream();

        const bool observerOnly = _engine->haveDeflectPixelOp();
        if (deflectEnabled && !_stream && _startStream(observerOnly))
            _sendSizeHints(*_engine);

        if (deflectEnabled && _stream && _stream->isConnected())
            _handleDeflectEvents();
    }

    void postRender()
    {
        const bool observerOnly = _engine->haveDeflectPixelOp();
        if (_params.getEnabled() && _stream && _stream->isConnected())
        {
            if (!observerOnly)
                _sendDeflectFrame(*_engine);
        }
    }

private:
    struct Image
    {
        std::vector<char> data;
        Vector2ui size;
        FrameBufferFormat format;
    };

    bool _startStream(const bool observerOnly)
    {
        try
        {
            if (observerOnly)
            {
                _stream.reset(new deflect::Observer(_params.getId(),
                                                    _params.getHostname(),
                                                    _params.getPort()));
            }
            else
            {
                _stream.reset(new deflect::Stream(_params.getId(),
                                                  _params.getHostname(),
                                                  _params.getPort()));
            }

            if (_stream->isConnected())
                BRAYNS_INFO << "Deflect successfully connected to Tide on host "
                            << _stream->getHost() << std::endl;
            else
                BRAYNS_ERROR << "Deflect failed to connect to Tide on host "
                             << _stream->getHost() << std::endl;

            if (_stream->registerForEvents())
                _setupSocketListener();
            else
                BRAYNS_ERROR << "Deflect failed to register for events!"
                             << std::endl;

            _params.setId(_stream->getId());
            _params.setHost(_stream->getHost());
            return true;
        }
        catch (const std::runtime_error& ex)
        {
            BRAYNS_ERROR << "Deflect failed to initialize. " << ex.what()
                         << std::endl;
            _params.setEnabled(false);
            return false;
        }
    }

    void _closeStream()
    {
        BRAYNS_INFO << "Closing Deflect stream" << std::endl;

        _sendFuture.wait();
        _sendFuture = make_ready_future(true);
#ifdef BRAYNS_USE_LIBUV
        if (_pollHandle)
        {
            _pollHandle->stop();
            _pollHandle.reset();
        }
#endif
        _stream.reset();
    }

    void _setupSocketListener()
    {
#ifdef BRAYNS_USE_LIBUV
        auto loop = uvw::Loop::getDefault();
        _pollHandle = loop->resource<uvw::PollHandle>(_stream->getDescriptor());

        _pollHandle->on<uvw::PollEvent>([engine = _engine](const auto&, auto&) {
            engine->triggerRender();
        });

        _pollHandle->start(uvw::PollHandle::Event::READABLE);
#endif
    }

    void _handleDeflectEvents()
    {
        while (_stream->hasEvent())
        {
            const deflect::Event& event = _stream->getEvent();
            switch (event.type)
            {
            case deflect::Event::EVT_PRESS:
                _previousPos =
                    _getWindowPos(event, _engine->getFrameBuffer().getSize());
                _pan = _pinch = false;
                break;
            case deflect::Event::EVT_MOVE:
            case deflect::Event::EVT_RELEASE:
            {
                const auto pos =
                    _getWindowPos(event, _engine->getFrameBuffer().getSize());
                if (!_pan && !_pinch)
                    _cameraManipulator.dragLeft(pos, _previousPos);
                _previousPos = pos;
                _pan = _pinch = false;
                break;
            }
            case deflect::Event::EVT_PAN:
            {
                if (_pinch)
                    break;
                const auto pos =
                    _getWindowPos(event, _engine->getFrameBuffer().getSize());
                _cameraManipulator.dragMiddle(pos, _previousPos);
                _previousPos = pos;
                _pan = true;
                break;
            }
            case deflect::Event::EVT_PINCH:
            {
                if (_pan)
                    break;
                const auto pos =
                    _getWindowPos(event, _engine->getFrameBuffer().getSize());
                const auto delta =
                    _getZoomDelta(event, _engine->getFrameBuffer().getSize());
                _cameraManipulator.wheel(pos, delta * wheelFactor);
                _pinch = true;
                break;
            }
            case deflect::Event::EVT_KEY_PRESS:
            {
                _keyboardHandler.handleKeyboardShortcut(event.text[0]);
                break;
            }
            case deflect::Event::EVT_VIEW_SIZE_CHANGED:
            {
                Vector2ui newSize(event.dx, event.dy);
                if (_engine->getCamera().isSideBySideStereo())
                    newSize.x() *= 2;

                if (_params.getResizing())
                {
                    _appParams.setWindowSize(
                        _engine->getSupportedFrameSize(newSize));
                }
                break;
            }
            case deflect::Event::EVT_CLOSE:
            {
                _params.setEnabled(false);
                _params.setHost("");
                _previousHost.clear();

                _closeStream();
                return;
            }
            default:
                break;
            }
        }
    }

    void _sendSizeHints(Engine& engine)
    {
        const auto size =
            engine.getSupportedFrameSize(_appParams.getWindowSize());
        const auto minSize = engine.getMinimumFrameSize();

        auto sizeHints = deflect::SizeHints();
        sizeHints.maxWidth = std::numeric_limits<unsigned int>::max();
        sizeHints.maxHeight = std::numeric_limits<unsigned int>::max();
        sizeHints.preferredWidth = size.x();
        sizeHints.preferredHeight = size.y();
        sizeHints.minWidth = minSize.x();
        sizeHints.minHeight = minSize.y();

        if (_engine->getCamera().isSideBySideStereo())
        {
            sizeHints.preferredWidth /= 2;
            sizeHints.minWidth /= 2;
        }
        _stream->sendSizeHints(sizeHints);
    }

    void _sendDeflectFrame(Engine& engine)
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
        frameBuffer.map();
        if (frameBuffer.getColorBuffer())
        {
            _copyToLastImage(frameBuffer);
            _sendFuture = _sendLastImage();
        }
        else
            _sendFuture = make_ready_future(true);
        frameBuffer.unmap();
    }

    void _copyToLastImage(FrameBuffer& frameBuffer)
    {
        const auto& size = frameBuffer.getSize();
        const size_t bufferSize =
            size.x() * size.y() * frameBuffer.getColorDepth();
        void* data = frameBuffer.getColorBuffer();

        _lastImage.data.resize(bufferSize);
        memcpy(_lastImage.data.data(), data, bufferSize);
        _lastImage.size = size;
        _lastImage.format = frameBuffer.getFrameBufferFormat();
    }

    deflect::Stream::Future _sendLastImage()
    {
        const auto format = _getDeflectImageFormat(_lastImage.format);

        deflect::ImageWrapper deflectImage(_lastImage.data.data(),
                                           _lastImage.size.x(),
                                           _lastImage.size.y(), format);
        if (_engine->getCamera().isSideBySideStereo())
            deflectImage.view = deflect::View::side_by_side;

        deflectImage.compressionQuality = _params.getQuality();
        deflectImage.compressionPolicy = _params.getCompression()
                                             ? deflect::COMPRESSION_ON
                                             : deflect::COMPRESSION_OFF;
        deflectImage.rowOrder = deflect::RowOrder::bottom_up;

        return static_cast<deflect::Stream&>(*_stream).sendAndFinish(
            deflectImage);
    }

    deflect::PixelFormat _getDeflectImageFormat(
        const FrameBufferFormat format) const
    {
        switch (format)
        {
        case FrameBufferFormat::bgra_i8:
            return deflect::BGRA;
        case FrameBufferFormat::rgb_i8:
            return deflect::RGB;
        default:
            return deflect::RGBA;
        }
    }

    Vector2d _getWindowPos(const deflect::Event& event,
                           const Vector2ui& windowSize) const
    {
        return {event.mouseX * windowSize.x(), event.mouseY * windowSize.y()};
    }

    double _getZoomDelta(const deflect::Event& pinchEvent,
                         const Vector2ui& windowSize) const
    {
        const auto dx = pinchEvent.dx * windowSize.x();
        const auto dy = pinchEvent.dy * windowSize.y();
        return std::copysign(std::sqrt(dx * dx + dy * dy), dx + dy);
    }

    EnginePtr _engine;
    ApplicationParameters& _appParams;
    StreamParameters& _params;
    KeyboardHandler& _keyboardHandler;
    AbstractManipulator& _cameraManipulator;
    Vector2d _previousPos;
    bool _pan = false;
    bool _pinch = false;
    std::unique_ptr<deflect::Observer> _stream;
    std::string _previousHost;
    Image _lastImage;
    deflect::Stream::Future _sendFuture;

#ifdef BRAYNS_USE_LIBUV
    std::shared_ptr<uvw::PollHandle> _pollHandle;
#endif
};

DeflectPlugin::DeflectPlugin(EnginePtr engine, PluginAPI* api)
    : _impl{std::make_shared<Impl>(engine, api)}
{
}

void DeflectPlugin::preRender()
{
    _impl->preRender();
}

void DeflectPlugin::postRender(FrameBuffer&)
{
    _impl->postRender();
}
} // namespace brayns
