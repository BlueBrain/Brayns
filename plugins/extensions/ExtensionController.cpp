/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include "ExtensionController.h"

#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/camera/Camera.h>
#include <cmath>

#ifdef BRAYNS_USE_DEFLECT
#  include "DeflectManager.h"
#endif

#ifdef BRAYNS_USE_RESTBRIDGE
#  include <restbridge/RestBridge.h>
#  include <zeq/zeq.h>
#  include <zeq/hbp/hbp.h>
#  include <turbojpeg.h>
#  include <servus/uri.h>
#  include <boost/bind.hpp>
#endif

namespace brayns
{

/** Manages extensions
 */
class ExtensionControllerImpl
{
public:
    ExtensionControllerImpl(
        ApplicationParametersPtr applicationParameters,
        ExtensionParameters& extensionParameters)
    : _applicationParameters(applicationParameters)
    , _extensionParameters(extensionParameters)
#ifdef BRAYNS_USE_DEFLECT
    , _alpha(0)
    , _theta(0)
    , _previousCameraPosition(0.f, 0.f, 0.f)
#endif
    {
        _initialize();
#ifdef BRAYNS_USE_RESTBRIDGE
        _compressor =  tjInitCompress();
#endif

#ifdef BRAYNS_USE_DEFLECT
        _previousCameraPosition.z() =
            -_extensionParameters.scene->getWorldBounds().getDimension().z();
#endif

    }

    ~ExtensionControllerImpl()
    {
#ifdef BRAYNS_USE_RESTBRIDGE
        if(_compressor)
            tjDestroy(_compressor);
#endif
    }

    void execute()
    {
#ifdef BRAYNS_USE_RESTBRIDGE
        if(_rcPublisher)
            _rcPublisher->publish( zeq::Event( zeq::vocabulary::EVENT_HEARTBEAT ));

        if(_rcSubscriber)
            while(_rcSubscriber->receive(0)) {}
#endif

#ifdef BRAYNS_USE_DEFLECT
        if(_deflectManager)
        {
            _sendDeflectFrame();
            _handleDeflectEvents();
        }
#endif
    }

    ExtensionParameters& getParameters()
    {
        return _extensionParameters;
    }

private:
    void _initialize()
    {
#ifdef BRAYNS_USE_DEFLECT
        _initializeDeflect();
#endif

#ifdef BRAYNS_USE_RESTBRIDGE
        _initializeRest();
#endif
    }

#ifdef BRAYNS_USE_DEFLECT
    void _initializeDeflect()
    {
        if(!_deflectManager && _applicationParameters->getDeflectHostname() != "")
        {
            _deflectManager.reset(new DeflectManager(
                                      _applicationParameters->getDeflectHostname(),
                                      _applicationParameters->getDeflectStreamname(),
                                      true, 100 ));
        }
    }

    void _sendDeflectFrame()
    {
        if(!_deflectManager) return;

        const Vector2i frameSize = _extensionParameters.frameBuffer->getSize();
        _deflectManager->send(
            frameSize, (uint32*)_extensionParameters.frameBuffer->getColorBuffer(), false);
    }

    void _handleDeflectEvents()
    {
        if(!_deflectManager) return;

        Vector2f position(0.f, 0.f);
        Vector2f wheelDelta(0.f, 0.f);
        const Vector2i frameSize = _extensionParameters.frameBuffer->getSize();
        bool pressed = false;
        bool closeApplication = false;

        if(_deflectManager->handleTouchEvents(position, wheelDelta, pressed, closeApplication))
        {
            if(pressed)
                _previousCameraPosition = Vector3f(position.x(), position.y(), 0.f);
            else
            {
                if(position.length()>std::numeric_limits<float>::epsilon() ||
                   wheelDelta.y()>std::numeric_limits<float>::epsilon() )
                {
                    const Vector3f& center =
                        _extensionParameters.scene->getWorldBounds().getCenter();
                    const Vector3f& size =
                        _extensionParameters.scene->getWorldBounds().getDimension();

                    const float dx = _previousCameraPosition.x() - position.x();
                    const float dy = _previousCameraPosition.y() - position.y();

                    if(std::abs(dx)<=1.f)
                        _theta -= frameSize.x()/100.f*std::asin(dx);
                    if(std::abs(dy)<=1.f)
                        _alpha += frameSize.y()/100.f*std::asin(dy);

                    _previousCameraPosition.x() = position.x();
                    _previousCameraPosition.y() = position.y();
                    _previousCameraPosition.z() -= wheelDelta.y()*5.f;

                    if(dx!=0.f || dy!=0.f || wheelDelta.y()!=0.f)
                    {
                        Vector3f cameraPosition = size*Vector3f(
                            _previousCameraPosition.z()*std::cos(_alpha)*std::cos(_theta),
                            _previousCameraPosition.z()*std::sin(_alpha)*std::cos(_theta),
                            _previousCameraPosition.z()*std::sin(_theta));

                        BRAYNS_INFO << "[" << _alpha << ", " << _theta << "] " << cameraPosition << std::endl;

                        _extensionParameters.camera->set(
                            center + cameraPosition,
                            center,
                            _extensionParameters.camera->getUp());

                       _extensionParameters.frameBuffer->clear();
                    }
                }
            }
        }
    }
#endif

#ifdef BRAYNS_USE_RESTBRIDGE
    void _initializeRest()
    {
        if(_rcPublisher)
            return;

        zeq::URI zeqSchema(_applicationParameters->getZeqSchema());
        _rcPublisher.reset(new ::zeq::Publisher(zeqSchema));

        Arguments args = _applicationParameters->getArguments();
        char** argv= new char*[args.size()];
        for( size_t i(0); i<args.size(); ++i )
            argv[i] = const_cast<char *>(args[i].c_str());
        _restBridge = restbridge::RestBridge::parse(*_rcPublisher, args.size(), argv);
        delete [] argv;

        if(_restBridge && !_rcSubscriber)
        {
            BRAYNS_INFO << "Registering handlers on " << _restBridge->getSubscriberURI() << std::endl;

            _rcSubscriber.reset(new ::zeq::Subscriber( _restBridge->getSubscriberURI() ));

            _rcSubscriber->registerHandler( ::zeq::hbp::EVENT_CAMERA,
                                            std::bind( &ExtensionControllerImpl::_onCamera,
                                                       this, std::placeholders::_1 ));

            _rcSubscriber->registerHandler( ::zeq::vocabulary::EVENT_REQUEST,
                                            std::bind( &ExtensionControllerImpl::_onRequest,
                                                       this, std::placeholders::_1 ));

            _rcSubscriber->registerHandler( ::zeq::vocabulary::EVENT_EXIT,
                                            std::bind( &ExtensionControllerImpl::_onExit,
                                                       this, std::placeholders::_1 ));
        }
    }

    void _onCamera(const ::zeq::Event& event)
    {
        if(event.getType() != zeq::hbp::EVENT_CAMERA)
            return;

        const std::vector< float >& matrix = zeq::hbp::deserializeCamera(event);
        Vector3f position(matrix[0], matrix[1], matrix[2]);
        Vector3f target(matrix[3], matrix[4], matrix[5]);
        Vector3f up(matrix[6], matrix[7], matrix[8]);

        _extensionParameters.camera->set(position, target, up);
        _extensionParameters.frameBuffer->clear();
    }

    void _onRequest(const ::zeq::Event& event)
    {
        const zeq::uint128_t& eventType =
                zeq::vocabulary::deserializeRequest( event );
        if(eventType == zeq::hbp::EVENT_IMAGEJPEG)
        {
            const Vector2i frameSize = _extensionParameters.frameBuffer->getSize();
            unsigned int* colorBuffer =
                (unsigned int*)_extensionParameters.frameBuffer->getColorBuffer();
            unsigned long jpegSize = frameSize.x()*frameSize.y()*sizeof(uint32);
            uint8_t* jpegData = _encodeJpeg(
                    (uint32_t)frameSize.x(),
                    (uint32_t)frameSize.y(),
                    (uint8_t*)colorBuffer, jpegSize);

            const zeq::hbp::data::ImageJPEG image( jpegSize, jpegData );
            const zeq::Event& image_event = zeq::hbp::serializeImageJPEG( image );
            _rcPublisher->publish( image_event );

            tjFree(jpegData);
        }
        else if(eventType == zeq::vocabulary::EVENT_VOCABULARY)
        {
            BRAYNS_INFO << "Registering application vocabulary" << std::endl;
            zeq::EventDescriptors vocabulary;

            vocabulary.push_back(zeq::EventDescriptor(
                                     zeq::hbp::IMAGEJPEG,
                                     zeq::hbp::EVENT_IMAGEJPEG,
                                     zeq::hbp::SCHEMA_IMAGEJPEG,
                                     zeq::PUBLISHER ) );

            vocabulary.push_back(zeq::EventDescriptor(
                                     zeq::hbp::CAMERA,
                                     zeq::hbp::EVENT_CAMERA,
                                     zeq::hbp::SCHEMA_CAMERA,
                                     zeq::SUBSCRIBER ) );

            vocabulary.push_back(zeq::EventDescriptor(
                                     zeq::hbp::CAMERA,
                                     zeq::hbp::EVENT_CAMERA,
                                     zeq::hbp::SCHEMA_CAMERA,
                                     zeq::PUBLISHER ) );

            const zeq::Event& vocEvent =
                    zeq::vocabulary::serializeVocabulary( vocabulary );

            _rcPublisher->publish( vocEvent );
        }
        else if(eventType == zeq::hbp::EVENT_CAMERA)
        {
            std::vector<float> matrix;

            Vector3f cameraPos = _extensionParameters.camera->getTarget();
            Vector3f cameraTarget = _extensionParameters.camera->getTarget();
            Vector3f cameraUp = _extensionParameters.camera->getUp();

            matrix.push_back(cameraPos.x());
            matrix.push_back(cameraPos.y());
            matrix.push_back(cameraPos.z());
            matrix.push_back(cameraTarget.x());
            matrix.push_back(cameraTarget.y());
            matrix.push_back(cameraTarget.z());
            matrix.push_back(cameraUp.x());
            matrix.push_back(cameraUp.y());
            matrix.push_back(cameraUp.z());
            for( size_t i=0; i<7; ++i) matrix.push_back(0);
            const zeq::Event& camera_event = zeq::hbp::serializeCamera( matrix );
            _rcPublisher->publish( camera_event );
        }
    }

    void _onExit( const zeq::Event &event )
    {
        if(event.getType() != zeq::vocabulary::EVENT_EXIT) return;
        // Not implemented
    }

    uint8_t* _encodeJpeg(const uint32_t width,
                         const uint32_t height,
                         const uint8_t* rawData,
                         unsigned long& dataSize)
    {
        uint8_t* tjSrcBuffer = const_cast< uint8_t* >(rawData);
        const int32_t pixelFormat = TJPF_RGBA;
        const int32_t color_components = 4; // Color Depth
        const int32_t tjPitch = width * color_components;
        const int32_t tjPixelFormat = pixelFormat;

        uint8_t* tjJpegBuf = 0;
        const int32_t tjJpegSubsamp = TJSAMP_444;
        const int32_t tjJpegQual = 100; // Image Quality
        const int32_t tjFlags = TJXOP_ROT180;

        const int32_t success =
                tjCompress2(_compressor, tjSrcBuffer, width, tjPitch, height,
                            tjPixelFormat, &tjJpegBuf, &dataSize, tjJpegSubsamp,
                            tjJpegQual, tjFlags);

        if(success != 0)
        {
            BRAYNS_INFO << "libjpeg-turbo image conversion failure" << std::endl;
            return 0;
        }
        return static_cast<uint8_t *>(tjJpegBuf);
    }
#endif

private:

    ApplicationParametersPtr _applicationParameters;
    ExtensionParameters _extensionParameters;

#ifdef BRAYNS_USE_DEFLECT
    std::unique_ptr<brayns::DeflectManager> _deflectManager;
    float _alpha;
    float _theta;
    Vector3f _previousCameraPosition;
#endif
#ifdef BRAYNS_USE_RESTBRIDGE
    tjhandle _compressor;
    std::unique_ptr<restbridge::RestBridge> _restBridge;
    std::unique_ptr<zeq::Subscriber> _rcSubscriber;
    std::unique_ptr<zeq::Publisher> _rcPublisher;
#endif

};

ExtensionController::ExtensionController(
        ApplicationParametersPtr applicationParameters,
        ExtensionParameters& extensionParameters )
  : _impl(new ExtensionControllerImpl(applicationParameters, extensionParameters))
{
}

ExtensionController::~ExtensionController()
{
}

void ExtensionController::execute()
{
    _impl->execute();
}

ExtensionParameters& ExtensionController::getParameters()
{
    return _impl->getParameters();
}

}
