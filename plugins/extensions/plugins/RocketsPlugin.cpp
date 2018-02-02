/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

// needs to be before RocketsPlugin.h to make template instantiation for
// _handleRPC work
#include "jsonSerialization.h"

#include "RocketsPlugin.h"

#include "jsonUtils.h"

#include <brayns/common/volume/VolumeHandler.h>

#include <fstream>

namespace
{
const std::string ENDPOINT_API_VERSION = "v1/";
const std::string ENDPOINT_APP_PARAMS = "application-parameters";
const std::string ENDPOINT_CAMERA = "camera";
const std::string ENDPOINT_CIRCUIT_CONFIG_BUILDER = "circuit-config-builder";
const std::string ENDPOINT_DATA_SOURCE = "data-source";
const std::string ENDPOINT_FRAME = "frame";
const std::string ENDPOINT_FRAME_BUFFERS = "frame-buffers";
const std::string ENDPOINT_GEOMETRY_PARAMS = "geometry-parameters";
const std::string ENDPOINT_IMAGE_JPEG = "image-jpeg";
const std::string ENDPOINT_MATERIAL_LUT = "material-lut";
const std::string ENDPOINT_PROGRESS = "progress";
const std::string ENDPOINT_RENDERING_PARAMS = "rendering-parameters";
const std::string ENDPOINT_SCENE = "scene";
const std::string ENDPOINT_SCENE_PARAMS = "scene-parameters";
const std::string ENDPOINT_SIMULATION_HISTOGRAM = "simulation-histogram";
const std::string ENDPOINT_STATISTICS = "statistics";
const std::string ENDPOINT_STREAM = "stream";
const std::string ENDPOINT_STREAM_TO = "stream-to";
const std::string ENDPOINT_VERSION = "version";
const std::string ENDPOINT_VOLUME_HISTOGRAM = "volume-histogram";
const std::string ENDPOINT_VOLUME_PARAMS = "volume-parameters";

const std::string METHOD_INSPECT = "inspect";
const std::string METHOD_RESET_CAMERA = "reset-camera";
const std::string METHOD_QUIT = "quit";

const std::string JSON_TYPE = "application/json";

const size_t NB_MAX_MESSAGES = 20; // Maximum number of network messages to read
                                   // between each rendering loop

std::string hyphenatedToCamelCase(const std::string& hyphenated)
{
    std::string camel = hyphenated;

    for (size_t x = 0; x < camel.length(); x++)
    {
        if (camel[x] == '-')
        {
            std::string tempString = camel.substr(x + 1, 1);

            transform(tempString.begin(), tempString.end(), tempString.begin(),
                      toupper);

            camel.erase(x, 2);
            camel.insert(x, tempString);
        }
    }
    camel[0] = toupper(camel[0]);
    return camel;
}
}

namespace brayns
{
RocketsPlugin::RocketsPlugin(ParametersManager& parametersManager)
    : ExtensionPlugin()
    , _parametersManager(parametersManager)
    , _imageGenerator(parametersManager.getApplicationParameters())
{
    _setupRocketsServer();
}

RocketsPlugin::~RocketsPlugin()
{
}

bool RocketsPlugin::run(EnginePtr engine, KeyboardHandler&,
                        AbstractManipulator&)
{
    if (!_rocketsServer)
        return true;

    if (_engine != engine)
    {
        _engine = engine;
        _registerEndpoints();
    }

    try
    {
        _broadcastWebsocketMessages();

        // In the case of interactions with Jupyter notebooks, HTTP messages are
        // received in a blocking and sequential manner, meaning that the
        // subscriber never has more than one message in its queue. In other
        // words, only one message is processed between each rendering loop. The
        // following code allows the processing of several messages and performs
        // rendering after NB_MAX_MESSAGES reads.
        for (size_t i = 0; i < NB_MAX_MESSAGES; ++i)
            _rocketsServer->process(0);
    }
    catch (const std::exception& exc)
    {
        BRAYNS_ERROR << "Error while handling HTTP/websocket messages: "
                     << exc.what() << std::endl;
    }

    return true;
}

std::string RocketsPlugin::_getHttpInterface() const
{
    const auto& params = _parametersManager.getApplicationParameters();
    const auto& args = params.arguments();
    for (int i = 0; i < (int)args.size() - 1; ++i)
    {
        if (args[i] == "--http-server")
            return args[i + 1];
    }
    return std::string();
}

void RocketsPlugin::_setupRocketsServer()
{
    try
    {
        _rocketsServer.reset(
            new rockets::Server{_getHttpInterface(), "rockets", 0});
        BRAYNS_INFO << "Rockets server running on " << _rocketsServer->getURI()
                    << std::endl;

        _jsonrpcServer.reset(new JsonRpcServer(*_rocketsServer));

        _parametersManager.getApplicationParameters().setHttpServerURI(
            _rocketsServer->getURI());
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << "Rockets server could not be initialized: '" << e.what()
                     << "'" << std::endl;
        return;
    }

    _setupWebsocket();
    _timer.start();
}

void RocketsPlugin::_setupWebsocket()
{
    _rocketsServer->handleOpen([this](const uintptr_t) {
        std::vector<rockets::ws::Response> responses;
        for (auto& i : _wsClientConnectNotifications)
            responses.push_back({i.second(), rockets::ws::Recipient::sender,
                                 rockets::ws::Format::text});

        if (_engine->isReady())
        {
            const auto image =
                _imageGenerator.createJPEG(_engine->getFrameBuffer());
            if (image.size > 0)
            {
                std::string message;
                message.assign((const char*)image.data.get(), image.size);
                responses.push_back({message, rockets::ws::Recipient::sender,
                                     rockets::ws::Format::binary});
            }
        }
        return responses;
    });
}

void RocketsPlugin::_broadcastWebsocketMessages()
{
    if (_rocketsServer->getConnectionCount() == 0)
        return;

    for (auto& op : _wsBroadcastOperations)
        op.second();
}

template <class T>
void RocketsPlugin::_handle(const std::string& endpoint, T& obj)
{
    _handleGET(endpoint, obj);
    _handlePUT(endpoint, obj);
}

template <class T, class F>
void RocketsPlugin::_handleGET(const std::string& endpoint, T& obj,
                               F modifiedFunc)
{
    using namespace rockets::http;

    _rocketsServer->handle(Method::GET, ENDPOINT_API_VERSION + endpoint,
                           [&obj](const Request&) {
                               return make_ready_response(Code::OK,
                                                          to_json(obj),
                                                          JSON_TYPE);
                           });

    _handleObjectSchema(endpoint, obj);

    _wsClientConnectNotifications[endpoint] = [this, &obj, endpoint] {
        return _jsonrpcServer->makeNotification(endpoint, obj);
    };

    _wsBroadcastOperations[endpoint] = [this, &obj, endpoint, modifiedFunc] {
        if (modifiedFunc(obj))
            _jsonrpcServer->emit(endpoint, obj);
    };
}

template <class T>
void RocketsPlugin::_handlePUT(const std::string& endpoint, T& obj)
{
    _handlePUT(endpoint, obj, std::function<void(T&)>());
}

template <class T, class F>
void RocketsPlugin::_handlePUT(const std::string& endpoint, T& obj,
                               F postUpdateFunc)
{
    using namespace rockets::http;
    _rocketsServer->handle(Method::PUT, ENDPOINT_API_VERSION + endpoint,
                           [&obj, postUpdateFunc](const Request& req) {
                               return make_ready_response(
                                   from_json(obj, req.body, postUpdateFunc)
                                       ? Code::OK
                                       : Code::BAD_REQUEST);
                           });

    _handleObjectSchema(endpoint, obj);

    _jsonrpcServer->bind(endpoint, [this, endpoint, &obj, postUpdateFunc](
                                       rockets::jsonrpc::Request request) {
        if (from_json(obj, request.message, postUpdateFunc))
        {
            const auto& msg = _jsonrpcServer->makeNotification(endpoint, obj);
            _rocketsServer->broadcastText(msg, {request.clientID});
            return rockets::jsonrpc::Response{"null"};
        }
        return rockets::jsonrpc::Response::invalidParams();
    });
}

template <class P, class R>
void RocketsPlugin::_handleRPC(const std::string& method,
                               const RpcDocumentation& doc,
                               std::function<R(P)> action)
{
    _jsonrpcServer->bind<P, R>(method, action);
    _handleSchema(method, buildJsonRpcSchema<P, R>(method, doc));
}

void RocketsPlugin::_handleRPC(const std::string& method,
                               const std::string& description,
                               std::function<void()> action)
{
    _jsonrpcServer->connect(method, action);
    _handleSchema(method, buildJsonRpcSchema(method, description));
}

template <class T>
void RocketsPlugin::_handleObjectSchema(const std::string& endpoint, T& obj)
{
    _handleSchema(endpoint, getSchema(obj, hyphenatedToCamelCase(endpoint)));
}

void RocketsPlugin::_handleSchema(const std::string& endpoint,
                                  const std::string& schema)
{
    using namespace rockets::http;
    _rocketsServer->handle(Method::GET,
                           ENDPOINT_API_VERSION + endpoint + "/schema",
                           [schema](const Request&) {
                               return make_ready_response(Code::OK, schema,
                                                          JSON_TYPE);
                           });
}

void RocketsPlugin::_registerEndpoints()
{
    _handleApplicationParams();
    _handleGeometryParams();
    _handleImageJPEG();
    _handleStreaming();
    _handleVersion();
    _handleVolumeParams();

    _handle(ENDPOINT_FRAME, _parametersManager.getAnimationParameters());
    _handle(ENDPOINT_RENDERING_PARAMS,
            _parametersManager.getRenderingParameters());
    _handle(ENDPOINT_SCENE_PARAMS, _parametersManager.getSceneParameters());

    _rocketsServer->handle(
        rockets::http::Method::GET,
        ENDPOINT_API_VERSION + ENDPOINT_CIRCUIT_CONFIG_BUILDER,
        std::bind(&RocketsPlugin::_handleCircuitConfigBuilder, this,
                  std::placeholders::_1));

    // following endpoints need a valid engine
    _handle(ENDPOINT_CAMERA, _engine->getCamera());
    _handleGET(ENDPOINT_PROGRESS, _engine->getProgress());
    _handle(ENDPOINT_MATERIAL_LUT, _engine->getScene().getTransferFunction());
    _handleGET(ENDPOINT_SCENE, _engine->getScene(), [&](const Scene& scene) {
        return _engine->isReady() && scene.getModified();
    });
    _handlePUT(ENDPOINT_SCENE, _engine->getScene(),
               [](Scene& scene) { scene.commitMaterials(Action::update); });
    _handleGET(ENDPOINT_STATISTICS, _engine->getStatistics(),
               [&](const Statistics& statistics) {
                   return _engine->isReady() && statistics.getModified();
               });

    _handleFrameBuffer();
    _handleSimulationHistogram();
    _handleVolumeHistogram();

    _handleInspect();
    _handleResetCamera();
    _handleQuit();
}

void RocketsPlugin::_handleApplicationParams()
{
    auto& params = _parametersManager.getApplicationParameters();
    auto postUpdate = [this](ApplicationParameters& params_) {
        if (params_.getFrameExportFolder().empty())
            _engine->resetFrameNumber();
    };
    _handleGET(ENDPOINT_APP_PARAMS, params);
    _handlePUT(ENDPOINT_APP_PARAMS, params, postUpdate);
}

void RocketsPlugin::_handleFrameBuffer()
{
    // don't add framebuffer to websockets for performance
    using namespace rockets::http;
    _rocketsServer->handleGET(ENDPOINT_API_VERSION + ENDPOINT_FRAME_BUFFERS,
                              _engine->getFrameBuffer());
    _handleObjectSchema(ENDPOINT_FRAME_BUFFERS, _engine->getFrameBuffer());
}

void RocketsPlugin::_handleGeometryParams()
{
    auto& params = _parametersManager.getGeometryParameters();
    auto postUpdate = [this](GeometryParameters&) {
        if (_engine->isReady())
            _engine->buildScene();
    };
    _handleGET(ENDPOINT_GEOMETRY_PARAMS, params);
    _handlePUT(ENDPOINT_GEOMETRY_PARAMS, params, postUpdate);
}

void RocketsPlugin::_handleImageJPEG()
{
    using namespace rockets::http;

    auto func = [&](const Request&) {
        if (!_engine->isReady())
            return make_ready_response(Code::NOT_SUPPORTED);
        const auto obj = _imageGenerator.createJPEG(_engine->getFrameBuffer());
        if (obj.size == 0)
            return make_ready_response(Code::BAD_REQUEST);
        return make_ready_response(Code::OK, to_json(obj), JSON_TYPE);
    };
    _rocketsServer->handle(Method::GET,
                           ENDPOINT_API_VERSION + ENDPOINT_IMAGE_JPEG, func);

    _rocketsServer->handle(
        Method::GET, ENDPOINT_API_VERSION + ENDPOINT_IMAGE_JPEG + "/schema",
        [&](const Request&) {
            ImageGenerator::ImageJPEG obj;
            return make_ready_response(Code::OK,
                                       getSchema(obj, hyphenatedToCamelCase(
                                                          ENDPOINT_IMAGE_JPEG)),
                                       JSON_TYPE);
        });

    _wsBroadcastOperations[ENDPOINT_IMAGE_JPEG] = [this] {
        if (_engine->isReady() && _engine->getRenderer().hasNewImage())
        {
            const auto fps = _parametersManager.getApplicationParameters()
                                 .getImageStreamFPS();
            if (_timer.elapsed() < 1.0 / fps)
                return;

            _timer.start();

            const auto image =
                _imageGenerator.createJPEG(_engine->getFrameBuffer());
            if (image.size > 0)
                _rocketsServer->broadcastBinary((const char*)image.data.get(),
                                                image.size);
        }
    };
}

void RocketsPlugin::_handleSimulationHistogram()
{
    Histogram tmp;
    _handleObjectSchema(ENDPOINT_SIMULATION_HISTOGRAM, tmp);

    using namespace rockets::http;

    auto func = [this](const Request&) {
        auto simulationHandler = _engine->getScene().getSimulationHandler();
        if (!simulationHandler)
            return make_ready_response(Code::NOT_SUPPORTED);
        const auto& histo = simulationHandler->getHistogram();
        return make_ready_response(Code::OK, to_json(histo), JSON_TYPE);
    };
    _rocketsServer->handle(Method::GET,
                           ENDPOINT_API_VERSION + ENDPOINT_SIMULATION_HISTOGRAM,
                           func);
}

void RocketsPlugin::_handleStreaming()
{
#if BRAYNS_USE_DEFLECT
    _handle(ENDPOINT_STREAM, _parametersManager.getStreamParameters());
    _handlePUT(ENDPOINT_STREAM_TO, _parametersManager.getStreamParameters());
#else
    _handleGET(ENDPOINT_STREAM, _parametersManager.getStreamParameters());
    using namespace rockets::http;
    auto respondNotImplemented = [](const Request&) {
        const auto message = "Brayns was not compiled with streaming support";
        return make_ready_response(Code::NOT_IMPLEMENTED, message);
    };
    _rocketsServer->handle(Method::PUT, ENDPOINT_STREAM, respondNotImplemented);
    _rocketsServer->handle(Method::PUT, ENDPOINT_STREAM_TO,
                           respondNotImplemented);
#endif
}

void RocketsPlugin::_handleVersion()
{
    static brayns::Version version;
    using namespace rockets::http;
    _rocketsServer->handleGET(ENDPOINT_API_VERSION + ENDPOINT_VERSION, version);
    _rocketsServer->handle(Method::GET,
                           ENDPOINT_API_VERSION + ENDPOINT_VERSION + "/schema",
                           [&](const Request&) {
                               return make_ready_response(Code::OK,
                                                          version.getSchema(),
                                                          JSON_TYPE);
                           });
    _wsClientConnectNotifications[ENDPOINT_VERSION] = [this] {
        return _jsonrpcServer->makeNotification(ENDPOINT_VERSION, version);
    };
}

void RocketsPlugin::_handleVolumeHistogram()
{
    Histogram tmp;
    _handleObjectSchema(ENDPOINT_VOLUME_HISTOGRAM, tmp);

    using namespace rockets::http;

    auto func = [this](const Request&) {
        auto volumeHandler = _engine->getScene().getVolumeHandler();
        if (!volumeHandler)
            return make_ready_response(Code::NOT_SUPPORTED);
        const auto& histo = volumeHandler->getHistogram();
        return make_ready_response(Code::OK, to_json(histo), JSON_TYPE);
    };

    _rocketsServer->handle(Method::GET,
                           ENDPOINT_API_VERSION + ENDPOINT_VOLUME_HISTOGRAM,
                           func);
}

void RocketsPlugin::_handleVolumeParams()
{
    auto& params = _parametersManager.getVolumeParameters();
    auto postUpdate = [this](VolumeParameters&) {
        if (_engine->isReady())
            _engine->buildScene();
    };
    _handleGET(ENDPOINT_VOLUME_PARAMS, params);
    _handlePUT(ENDPOINT_VOLUME_PARAMS, params, postUpdate);
}

void RocketsPlugin::_handleInspect()
{
    using Position = std::array<float, 2>;
    RpcDocumentation doc{"Inspect the scene at x-y position", "position",
                         "x-y position in normalized coordinates"};
    _handleRPC<Position, Renderer::PickResult>(
        METHOD_INSPECT, doc, [this](const Position& position) {
            return _engine->getRenderer().pick(
                Vector2f(position[0], position[1]));
        });
}

void RocketsPlugin::_handleQuit()
{
    _handleRPC(METHOD_QUIT, "Quit the application",
               [this] { _engine->setKeepRunning(false); });
}

void RocketsPlugin::_handleResetCamera()
{
    _handleRPC(METHOD_RESET_CAMERA, "Resets the camera to its initial values",
               [this] {
                   _engine->getCamera().reset();
                   _jsonrpcServer->emit(ENDPOINT_CAMERA, _engine->getCamera());
               });
}

std::future<rockets::http::Response> RocketsPlugin::_handleCircuitConfigBuilder(
    const rockets::http::Request& request)
{
    using namespace rockets::http;

    const auto& params = _parametersManager.getApplicationParameters();
    const auto filename = params.getTmpFolder() + "/BlueConfig";
    if (_writeBlueConfigFile(filename, request.query))
    {
        const std::string body = "{\"filename\":\"" + filename + "\"}";
        return make_ready_response(Code::OK, body, JSON_TYPE);
    }
    return make_ready_response(Code::SERVICE_UNAVAILABLE);
}

bool RocketsPlugin::_writeBlueConfigFile(
    const std::string& filename,
    const std::map<std::string, std::string>& params)
{
    std::ofstream blueConfig(filename);
    if (!blueConfig.good())
        return false;

    std::map<std::string, std::string> dictionary = {
        {"morphology_folder", "MorphologyPath"}, {"mvd_file", "CircuitPath"}};

    blueConfig << "Run Default" << std::endl << "{" << std::endl;
    for (const auto& kv : params)
    {
        if (dictionary.find(kv.first) == dictionary.end())
        {
            BRAYNS_ERROR << "BlueConfigBuilder: Unknown parameter " << kv.first
                         << std::endl;
            continue;
        }
        blueConfig << dictionary[kv.first] << " " << kv.second << std::endl;
    }
    blueConfig << "}" << std::endl;
    blueConfig.close();
    return true;
}
}
