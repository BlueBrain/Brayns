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

uint8_t _getChannel(const std::string& name)
{
    if (name.length() == 2)
        return std::atoi(&name.at(0));
    return 0;
}
}

namespace brayns
{
class DeflectPlugin::Impl
{
public:
    Impl(PluginAPI* api)
        : _engine(api->getEngine())
        , _appParams{api->getParametersManager().getApplicationParameters()}
        , _params{api->getParametersManager().getStreamParameters()}
        , _keyboardHandler(api->getKeyboardHandler())
        , _cameraManipulator(api->getCameraManipulator())
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

        const bool observerOnly = _engine.haveDeflectPixelOp();
        if (deflectEnabled && !_stream && _startStream(observerOnly))
            _sendSizeHints(_engine);

        if (deflectEnabled && _stream && _stream->isConnected())
            _handleDeflectEvents();
    }

    void postRender()
    {
        const bool observerOnly = _engine.haveDeflectPixelOp();
        if (_params.getEnabled() && _stream && _stream->isConnected())
        {
            if (!observerOnly)
                _sendDeflectFrame(_engine);
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

        _waitOnFutures();
        _lastImages.clear();
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

        _pollHandle->on<uvw::PollEvent>([& engine = _engine](const auto&,
                                                             auto&) {
            engine.triggerRender();
        });

        _pollHandle->start(uvw::PollHandle::Event::READABLE);
#endif
    }

    void _handleDeflectEvents()
    {
        const auto& windowSize = _appParams.getWindowSize();
        while (_stream->hasEvent())
        {
            const deflect::Event& event = _stream->getEvent();
            switch (event.type)
            {
            case deflect::Event::EVT_PRESS:
                _previousPos = _getWindowPos(event, windowSize);
                _pan = _pinch = false;
                break;
            case deflect::Event::EVT_MOVE:
            case deflect::Event::EVT_RELEASE:
            {
                const auto pos = _getWindowPos(event, windowSize);
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
                const auto pos = _getWindowPos(event, windowSize);
                _cameraManipulator.dragMiddle(pos, _previousPos);
                _previousPos = pos;
                _pan = true;
                break;
            }
            case deflect::Event::EVT_PINCH:
            {
                if (_pan)
                    break;
                const auto pos = _getWindowPos(event, windowSize);
                const auto delta = _getZoomDelta(event, windowSize);
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
                if (_params.getResizing())
                    _appParams.setWindowSize(newSize);
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
        const auto& frameBuffers = engine.getFrameBuffers();
        if (frameBuffers.empty())
            return;

        const auto& minSize = engine.getMinimumFrameSize();

        auto sizeHints = deflect::SizeHints();
        sizeHints.maxWidth = std::numeric_limits<unsigned int>::max();
        sizeHints.maxHeight = std::numeric_limits<unsigned int>::max();
        sizeHints.minWidth = minSize.x();
        sizeHints.minHeight = minSize.y();

        // only send preferred size if we have no multi-channel setup (e.g.
        // OpenDeck)
        const uint8_t channel = _getChannel(frameBuffers[0]->getName());
        Vector2ui preferredSize = frameBuffers[0]->getSize();
        for (auto frameBuffer : frameBuffers)
        {
            if (channel != _getChannel(frameBuffer->getName()))
            {
                preferredSize = {0, 0};
                break;
            }
        }

        sizeHints.preferredWidth = preferredSize.x();
        sizeHints.preferredHeight = preferredSize.y();
        _stream->sendSizeHints(sizeHints);
    }

    void _sendDeflectFrame(Engine& engine)
    {
        const bool error = _waitOnFutures();

        if (error)
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

        const auto& frameBuffers = engine.getFrameBuffers();
        for (size_t i = 0; i < frameBuffers.size(); ++i)
        {
            auto frameBuffer = frameBuffers[i];
            frameBuffer->map();
            if (frameBuffer->getColorBuffer())
            {
                const deflect::View view = _getView(frameBuffer->getName());
                const uint8_t channel = _getChannel(frameBuffer->getName());

                if (i <= _lastImages.size())
                    _lastImages.push_back({});
                auto& image = _lastImages[i];
                _copyToImage(image, *frameBuffer);
                _futures.push_back(_sendImage(image, view, channel));
            }
            frameBuffer->unmap();
        }
        _futures.push_back(
            static_cast<deflect::Stream&>(*_stream).finishFrame());
    }

    deflect::View _getView(const std::string& name) const
    {
        if (name.length() == 2)
        {
            if (name.at(1) == 'L')
                return deflect::View::left_eye;
            if (name.at(1) == 'R')
                return deflect::View::right_eye;
            return deflect::View::mono;
        }
        return deflect::View::mono;
    }

    void _copyToImage(Image& image, FrameBuffer& frameBuffer)
    {
        const auto& size = frameBuffer.getSize();
        const size_t bufferSize =
            size.x() * size.y() * frameBuffer.getColorDepth();
        const auto data = frameBuffer.getColorBuffer();

        image.data.resize(bufferSize);
        memcpy(image.data.data(), data, bufferSize);
        image.size = size;
        image.format = frameBuffer.getFrameBufferFormat();
    }

    deflect::Stream::Future _sendImage(const Image& image,
                                       const deflect::View& view,
                                       const uint8_t channel)
    {
        const auto format = _getDeflectImageFormat(image.format);

        deflect::ImageWrapper deflectImage(image.data.data(), image.size.x(),
                                           image.size.y(), format);

        deflectImage.view = view;
        deflectImage.channel = channel;
        deflectImage.compressionQuality = _params.getQuality();
        deflectImage.compressionPolicy = _params.getCompression()
                                             ? deflect::COMPRESSION_ON
                                             : deflect::COMPRESSION_OFF;
        deflectImage.rowOrder = deflect::RowOrder::bottom_up;

        return static_cast<deflect::Stream&>(*_stream).send(deflectImage);
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

    bool _waitOnFutures()
    {
        bool error = false;
        for (auto& future : _futures)
        {
            if (!future.get())
                error = true;
        }
        _futures.clear();
        return error;
    }

    Engine& _engine;
    ApplicationParameters& _appParams;
    StreamParameters& _params;
    KeyboardHandler& _keyboardHandler;
    AbstractManipulator& _cameraManipulator;
    Vector2d _previousPos;
    bool _pan = false;
    bool _pinch = false;
    std::unique_ptr<deflect::Observer> _stream;
    std::string _previousHost;
    std::vector<Image> _lastImages;
    std::vector<deflect::Stream::Future> _futures;

#ifdef BRAYNS_USE_LIBUV
    std::shared_ptr<uvw::PollHandle> _pollHandle;
#endif
};

void DeflectPlugin::init(PluginAPI* api)
{
    _impl = std::make_shared<Impl>(api);
}

void DeflectPlugin::preRender()
{
    _impl->preRender();
}

void DeflectPlugin::postRender()
{
    _impl->postRender();
}
}
