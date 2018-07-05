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

#include <brayns/common/Timer.h>
#include <brayns/common/tasks/Task.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brayns/tasks/AddModelFromBlobTask.h>
#include <brayns/tasks/AddModelTask.h>

#ifdef BRAYNS_USE_LIBUV
#include <uvw.hpp>
#endif

#include <rockets/jsonrpc/helpers.h>
#include <rockets/jsonrpc/server.h>
#include <rockets/server.h>

#include "BinaryRequests.h"
#include "ImageGenerator.h"

namespace
{
// REST PUT & GET, JSONRPC set-* notification, JSONRPC get-* request
const std::string ENDPOINT_ANIMATION_PARAMS = "animation-parameters";
const std::string ENDPOINT_APP_PARAMS = "application-parameters";
const std::string ENDPOINT_CAMERA = "camera";
const std::string ENDPOINT_GEOMETRY_PARAMS = "geometry-parameters";
const std::string ENDPOINT_TRANSFER_FUNCTION = "transfer-function";
const std::string ENDPOINT_RENDERING_PARAMS = "rendering-parameters";
const std::string ENDPOINT_SCENE = "scene";
const std::string ENDPOINT_SCENE_PARAMS = "scene-parameters";
const std::string ENDPOINT_STREAM = "stream";
const std::string ENDPOINT_VOLUME_PARAMS = "volume-parameters";

// REST GET, JSONRPC get-* request
const std::string ENDPOINT_STATISTICS = "statistics";
const std::string ENDPOINT_VERSION = "version";

// REST GET
const std::string ENDPOINT_FRAME_BUFFERS = "frame-buffers";
const std::string ENDPOINT_SIMULATION_HISTOGRAM = "simulation-histogram";
const std::string ENDPOINT_VOLUME_HISTOGRAM = "volume-histogram";

// JSONRPC async requests
const std::string METHOD_ADD_MODEL = "add-model";
const std::string METHOD_SNAPSHOT = "snapshot";
// METHOD_REQUEST_MODEL_UPLOAD from BinaryRequests.h

// JSONRPC synchronous requests
const std::string METHOD_GET_INSTANCES = "get-instances";
const std::string METHOD_IMAGE_JPEG = "image-jpeg";
const std::string METHOD_INSPECT = "inspect";
const std::string METHOD_REMOVE_MODEL = "remove-model";
const std::string METHOD_SCHEMA = "schema";
const std::string METHOD_UPDATE_INSTANCE = "update-instance";
const std::string METHOD_UPDATE_MODEL = "update-model";

// JSONRPC notifications
const std::string METHOD_CHUNK = "chunk";
const std::string METHOD_QUIT = "quit";
const std::string METHOD_RESET_CAMERA = "reset-camera";
const std::string METHOD_STREAM_TO = "stream-to";

const std::string JSON_TYPE = "application/json";

using Response = rockets::jsonrpc::Response;

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

std::string getNotificationEndpointName(const std::string& endpoint)
{
    return "set-" + endpoint;
}

std::string getRequestEndpointName(const std::string& endpoint)
{
    return "get-" + endpoint;
}
}

namespace brayns
{
template <class T, class F>
inline bool from_json(T& obj, const std::string& json, F postUpdateFunc = [] {})
{
    staticjson::ParseStatus status;
    const auto success =
        staticjson::from_json_string(json.c_str(), &obj, &status);
    if (success)
    {
        obj.markModified();
        if (std::function<void(T&)>(postUpdateFunc))
            postUpdateFunc(obj);
    }
    else
        BRAYNS_ERROR << status.description() << std::endl;
    return success;
}

class RocketsPlugin::Impl
{
public:
    Impl(EnginePtr engine, PluginAPI* api)
        : _engine(engine)
        , _parametersManager(api->getParametersManager())
    {
        _setupRocketsServer();
    }

    ~Impl()
    {
        // cancel all pending tasks; cancel() will remove itself from _tasks
        while (!_tasks.empty())
        {
            auto task = _tasks.begin()->second;
            _tasks.begin()->first->cancel();
            task->wait();
        }

        if (_rocketsServer)
            _rocketsServer->setSocketListener(nullptr);
    }

    void preRender()
    {
        if (!_rocketsServer || !_manualProcessing)
            return;

        // https://github.com/BlueBrain/Brayns/issues/342
        // WAR: modifications by braynsViewer have to be broadcasted. Don't do
        // this for braynsService, as otherwise messages that arrive while we're
        // rendering (async rendering!) are re-broadcasted.
        _broadcastWebsocketMessages();

        try
        {
            _rocketsServer->process(0);
        }
        catch (const std::exception& exc)
        {
            BRAYNS_ERROR << "Error while handling HTTP/websocket messages: "
                         << exc.what() << std::endl;
        }
    }

    void postRender()
    {
        if (!_rocketsServer || _rocketsServer->getConnectionCount() == 0)
            return;

        // only broadcast changes that are a result of the rendering. All other
        // changes are already broadcasted in preRender().
        _wsBroadcastOperations[ENDPOINT_ANIMATION_PARAMS]();
        _wsBroadcastOperations[METHOD_IMAGE_JPEG]();
        _wsBroadcastOperations[ENDPOINT_STATISTICS]();
    }

    void postSceneLoading()
    {
        if (!_rocketsServer)
            return;

        // simulation was potentially loaded
        _wsBroadcastOperations[ENDPOINT_ANIMATION_PARAMS]();
        _wsBroadcastOperations[ENDPOINT_SCENE]();      // bounds
        _wsBroadcastOperations[ENDPOINT_STATISTICS](); // scene size
    }

    void _setupRocketsServer()
    {
        try
        {
            const auto& appParams =
                _parametersManager.getApplicationParameters();
#ifdef BRAYNS_USE_LIBUV
            if (uvw::Loop::getDefault()->alive())
            {
                _rocketsServer = std::make_unique<rockets::Server>(
                    uv_default_loop(), appParams.getHttpServerURI(), "rockets");
                _manualProcessing = false;
            }
            else
#endif
                _rocketsServer = std::make_unique<rockets::Server>(
                    appParams.getHttpServerURI(), "rockets", 0);

            BRAYNS_INFO << "Rockets server running on "
                        << _rocketsServer->getURI() << std::endl;

            _jsonrpcServer = std::make_unique<JsonRpcServer>(*_rocketsServer);

            _parametersManager.getApplicationParameters().setHttpServerURI(
                _rocketsServer->getURI());
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << "Rockets server could not be initialized: '"
                         << e.what() << "'" << std::endl;
            return;
        }

        _setupWebsocket();
        _registerEndpoints();
        _timer.start();
    }

    void _setupWebsocket()
    {
        _rocketsServer->handleOpen([this](const uintptr_t) {
            std::vector<rockets::ws::Response> responses;

            const auto image =
                _imageGenerator.createJPEG(_engine->getFrameBuffer(),
                                           _parametersManager
                                               .getApplicationParameters()
                                               .getJpegCompression());
            if (image.size > 0)
            {
                std::string message;
                message.assign((const char*)image.data.get(), image.size);
                responses.push_back({message, rockets::ws::Recipient::sender,
                                     rockets::ws::Format::binary});
            }
            return responses;
        });

        _rocketsServer->handleClose([this](const uintptr_t clientID) {
            _binaryRequests.removeRequest(clientID);
            return std::vector<rockets::ws::Response>{};
        });

        _rocketsServer->handleBinary(std::bind(&BinaryRequests::processMessage,
                                               std::ref(_binaryRequests),
                                               std::placeholders::_1));
    }

    void _broadcastWebsocketMessages()
    {
        if (_rocketsServer->getConnectionCount() == 0)
            return;

        for (auto& op : _wsBroadcastOperations)
            op.second();
    }

    template <class T, class F>
    void _handleGET(const std::string& endpoint, T& obj, F modifiedFunc)
    {
        using namespace rockets::http;

        _rocketsServer->handle(Method::GET, endpoint, [&obj](const Request&) {
            return make_ready_response(Code::OK, to_json(obj), JSON_TYPE);
        });

        _handleObjectSchema(endpoint, obj);

        _wsBroadcastOperations[endpoint] =
            [& server = _jsonrpcServer, &obj, endpoint, modifiedFunc ]
        {
            if (modifiedFunc(obj))
                server->notify(getNotificationEndpointName(endpoint), obj);
        };

        const std::string rpcEndpoint = getRequestEndpointName(endpoint);

        _jsonrpcServer->bind(rpcEndpoint,
                             std::function<const T&()>(
                                 [&obj]() -> const T& { return obj; }));
        _handleSchema(rpcEndpoint,
                      buildJsonRpcSchema(rpcEndpoint,
                                         "Get the current state of " +
                                             endpoint));
    }

    template <class T>
    void _handleGET(const std::string& endpoint, T& obj)
    {
        _handleGET(endpoint, obj, [](const T& o) { return o.isModified(); });
    }

    template <class T>
    void _handlePUT(const std::string& endpoint, T& obj)
    {
        _handlePUT(endpoint, obj, std::function<void(T&)>());
    }

    template <class T, class F>
    void _handlePUT(const std::string& endpoint, T& obj, F postUpdateFunc)
    {
        using namespace rockets::http;
        _rocketsServer->handle(Method::PUT, endpoint, [&obj, postUpdateFunc](
                                                          const Request& req) {
            return make_ready_response(from_json(obj, req.body, postUpdateFunc)
                                           ? Code::OK
                                           : Code::BAD_REQUEST);
        });

        _handleObjectSchema(endpoint, obj);

        const std::string rpcEndpoint = getNotificationEndpointName(endpoint);

        _jsonrpcServer->bind(rpcEndpoint, [
            engine = _engine, &server = _rocketsServer, rpcEndpoint, &obj,
            postUpdateFunc
        ](rockets::jsonrpc::Request request) {
            if (from_json(obj, request.message, postUpdateFunc))
            {
                engine->triggerRender();

                const auto& msg =
                    rockets::jsonrpc::makeNotification(rpcEndpoint, obj);
                server->broadcastText(msg, {request.clientID});
                return rockets::jsonrpc::Response{"null"};
            }
            return rockets::jsonrpc::Response::invalidParams();
        });
        RpcDocumentation doc{"Set the new state of " + endpoint, "param",
                             endpoint};
        _handleSchema(rpcEndpoint,
                      buildJsonRpcSchema<T>(rpcEndpoint, doc, obj));
    }

    template <class T>
    void _handle(const std::string& endpoint, T& obj)
    {
        _handleGET(endpoint, obj);
        _handlePUT(endpoint, obj);
    }

    template <class P, class R>
    void _handleRPC(const std::string& method, const RpcDocumentation& doc,
                    std::function<R(P)> action)
    {
        _jsonrpcServer->bind<P, R>(method, action);
        _handleSchema(method, buildJsonRpcSchema<P, R>(method, doc));
    }

    template <class P>
    void _handleRPC(const std::string& method, const RpcDocumentation& doc,
                    std::function<void(P)> action)
    {
        _jsonrpcServer->connect<P>(method, action);
        _handleSchema(method, buildJsonRpcSchema<P>(method, doc));
    }

    void _handleRPC(const std::string& method, const std::string& description,
                    std::function<void()> action)
    {
        _jsonrpcServer->connect(method, action);
        _handleSchema(method, buildJsonRpcSchema(method, description));
    }

    template <class P, class R>
    void _handleAsyncRPC(const std::string& method, const RpcDocumentation& doc,
                         std::function<rockets::jsonrpc::CancelRequestCallback(
                             P, uintptr_t, rockets::jsonrpc::AsyncResponse,
                             rockets::jsonrpc::ProgressUpdateCallback)>
                             action)
    {
        _jsonrpcServer->bindAsync<P>(method, action);
        _handleSchema(method, buildJsonRpcSchema<P, R>(method, doc));
    }

    template <class P, class R>
    void _handleTask(
        const std::string& method, const RpcDocumentation& doc,
        std::function<std::shared_ptr<Task<R>>(P, uintptr_t)> createTask)
    {
        // define the action that is executed on every incoming request from the
        // client:
        // - create the task that shall be executed
        // - wire the result of task to the response callback from rockets
        // - setup progress reporting during the task execution using libuv
        // - wire the cancel request from rockets to the task

        auto action = [&tasks = _tasks, &binaryRequests = _binaryRequests,
                createTask,  &mutex = _tasksMutex]
                (P params, auto clientID, auto respond, auto progressCb)
        {
            // transform task error to rockets error response
            auto errorCallback = [respond](const TaskRuntimeError& error) {
                respond(
                    {Response::Error{error.what(), error.code, error.data}});
            };

            try
            {
                // transform task result to rockets response
                auto readyCallback = [respond](const R& result) {
                    try
                    {
                        respond({to_json(result)});
                    }
                    catch (const std::runtime_error& e)
                    {
                        respond({Response::Error{e.what(), -1}});
                    }
                };

                // create the task that shall be executed for this request
                auto task = createTask(std::move(params), clientID);

                std::function<void()> finishProgress = [task] {
                    task->progress.update("Done", 1.f);
                };

// setup periodic progress reporting if we have libuv running
#ifdef BRAYNS_USE_LIBUV
                if (uvw::Loop::getDefault()->alive())
                {
                    auto progressUpdate =
                        uvw::Loop::getDefault()->resource<uvw::TimerHandle>();

                    auto sendProgress =
                        [ progressCb, &progress = task->progress ]
                    {
                        progress.consume(progressCb);
                    };
                    progressUpdate->on<uvw::TimerEvent>(
                        [sendProgress](const auto&, auto&) { sendProgress(); });

                    finishProgress = [task, progressUpdate, sendProgress] {
                        task->progress.update("Done", 1.f);
                        sendProgress();
                        progressUpdate->stop();
                        progressUpdate->close();
                    };

                    progressUpdate->start(std::chrono::milliseconds(0),
                                          std::chrono::milliseconds(800));
                }
#endif

                // setup the continuation task that handles the result or error
                // of the task to handle the responses to rockets accordingly.
                auto responseTask = std::make_shared<async::task<void>>(
                    task->get().then([readyCallback, errorCallback, &tasks,
                                      &binaryRequests, task, finishProgress,
                                      &mutex](typename Task<R>::Type result) {
                        finishProgress();

                        try
                        {
                            readyCallback(result.get());
                        }
                        catch (const TaskRuntimeError& e)
                        {
                            errorCallback(e);
                        }
                        catch (const std::exception& e)
                        {
                            errorCallback({e.what()});
                        }
                        catch (const async::task_canceled&)
                        {
                            task->finishCancel();
                        }

                        std::lock_guard<std::mutex> lock(mutex);
                        tasks.erase(task);
                        binaryRequests.removeTask(task);
                    }));

                std::lock_guard<std::mutex> lock(mutex);
                tasks.emplace(task, responseTask);

                // forward the cancel request from rockets to the task
                auto cancel = [task, responseTask](auto done) {
                    task->cancel(done);
                };

                task->schedule();

                return rockets::jsonrpc::CancelRequestCallback(cancel);
            }
            // respond errors during the setup of the task
            catch (const BinaryTaskError& e)
            {
                errorCallback({e.what(), e.code, to_json(e.error)});
            }
            catch (const TaskRuntimeError& e)
            {
                errorCallback(e);
            }
            catch (const std::exception& e)
            {
                errorCallback({e.what()});
            }
            return rockets::jsonrpc::CancelRequestCallback();
        };
        _handleAsyncRPC<P, R>(method, doc, action);
    }

    template <class T>
    void _handleObjectSchema(const std::string& endpoint)
    {
        _handleSchema(endpoint, getSchema<T>(hyphenatedToCamelCase(endpoint)));
    }

    template <class T>
    void _handleObjectSchema(const std::string& endpoint, T& obj)
    {
        _handleSchema(endpoint,
                      getSchema(obj, hyphenatedToCamelCase(endpoint)));
    }

    void _handleSchema(const std::string& endpoint, const std::string& schema)
    {
        using namespace rockets::http;
        _rocketsServer->handle(Method::GET, endpoint + "/schema",
                               [schema](const Request&) {
                                   return make_ready_response(Code::OK, schema,
                                                              JSON_TYPE);
                               });

        _schemas[endpoint] = schema;
    }

    void _registerEndpoints()
    {
        _handleGeometryParams();
        _handleImageJPEG();
        _handleStreaming();
        _handleVersion();
        _handleVolumeParams();

        _handle(ENDPOINT_APP_PARAMS,
                _parametersManager.getApplicationParameters());
        _handle(ENDPOINT_ANIMATION_PARAMS,
                _parametersManager.getAnimationParameters());
        _handle(ENDPOINT_RENDERING_PARAMS,
                _parametersManager.getRenderingParameters());
        _handle(ENDPOINT_SCENE_PARAMS, _parametersManager.getSceneParameters());

        // following endpoints need a valid engine
        _handle(ENDPOINT_CAMERA, _engine->getCamera());
        _handle(ENDPOINT_TRANSFER_FUNCTION,
                _engine->getScene().getTransferFunction());
        _handle(ENDPOINT_SCENE, _engine->getScene());

        _handleGET(ENDPOINT_STATISTICS, _engine->getStatistics());

        _handleFrameBuffer();
        _handleSimulationHistogram();

        _handleSchemaRPC();

        _handleInspect();
        _handleQuit();
        _handleResetCamera();
        _handleSnapshot();
        _handleStreamTo();

        _handleRequestModelUpload();
        _handleChunk();

        _handleAddModel();
        _handleRemoveModel();
        _handleUpdateModel();

        _handleGetInstances();
        _handleUpdateInstance();
    }

    void _handleFrameBuffer()
    {
        // don't add framebuffer to websockets for performance
        using namespace rockets::http;
        _rocketsServer->handleGET(ENDPOINT_FRAME_BUFFERS,
                                  _engine->getFrameBuffer());
        _handleObjectSchema(ENDPOINT_FRAME_BUFFERS, _engine->getFrameBuffer());
    }

    void _handleGeometryParams()
    {
        auto& params = _parametersManager.getGeometryParameters();
        auto postUpdate = [this](GeometryParameters&) {
            _engine->markRebuildScene();
        };
        _handleGET(ENDPOINT_GEOMETRY_PARAMS, params);
        _handlePUT(ENDPOINT_GEOMETRY_PARAMS, params, postUpdate);
    }

    void _handleImageJPEG()
    {
        _jsonrpcServer->bind(METHOD_IMAGE_JPEG,
                             std::function<ImageGenerator::ImageBase64()>([&] {
                                 return _imageGenerator.createImage(
                                     _engine->getFrameBuffer(), "jpg",
                                     _parametersManager
                                         .getApplicationParameters()
                                         .getJpegCompression());
                             }));
        _handleSchema(METHOD_IMAGE_JPEG,
                      buildJsonRpcSchema(METHOD_IMAGE_JPEG,
                                         "Get the current state of " +
                                             METHOD_IMAGE_JPEG));

        _wsBroadcastOperations[METHOD_IMAGE_JPEG] = [this] {
            if (_engine->getFrameBuffer().isModified())
            {
                const auto& params =
                    _parametersManager.getApplicationParameters();
                const auto fps = params.getImageStreamFPS();
                if (fps == 0)
                    return;

                const auto elapsed = _timer.elapsed() + _leftover;
                const auto duration = 1.0 / fps;
                if (elapsed < duration)
                    return;

                _leftover = elapsed - duration;
                for (; _leftover > duration;)
                    _leftover -= duration;
                _timer.start();

                const auto image =
                    _imageGenerator.createJPEG(_engine->getFrameBuffer(),
                                               params.getJpegCompression());
                if (image.size > 0)
                    _rocketsServer->broadcastBinary(
                        (const char*)image.data.get(), image.size);
            }
        };
    }

    void _handleSimulationHistogram()
    {
        _handleObjectSchema<Histogram>(ENDPOINT_SIMULATION_HISTOGRAM);

        using namespace rockets::http;

        auto func = [this](const Request&) {
            auto simulationHandler = _engine->getScene().getSimulationHandler();
            if (!simulationHandler)
                return make_ready_response(Code::NOT_SUPPORTED);
            const auto& histo = simulationHandler->getHistogram();
            return make_ready_response(Code::OK, to_json(histo), JSON_TYPE);
        };
        _rocketsServer->handle(Method::GET, ENDPOINT_SIMULATION_HISTOGRAM,
                               func);
    }

    void _handleStreaming()
    {
#if BRAYNS_USE_DEFLECT
        _handle(ENDPOINT_STREAM, _parametersManager.getStreamParameters());
#else
        _handleGET(ENDPOINT_STREAM, _parametersManager.getStreamParameters());
        using namespace rockets::http;
        auto respondNotImplemented = [](const Request&) {
            const auto message =
                "Brayns was not compiled with streaming support";
            return make_ready_response(Code::NOT_IMPLEMENTED, message);
        };
        _rocketsServer->handle(Method::PUT, ENDPOINT_STREAM,
                               respondNotImplemented);
#endif
    }

    void _handleVersion()
    {
        static brayns::Version version;
        using namespace rockets::http;
        _rocketsServer->handleGET(ENDPOINT_VERSION, version);
        _rocketsServer->handle(
            Method::GET, ENDPOINT_VERSION + "/schema", [&](const Request&) {
                return make_ready_response(Code::OK, version.getSchema(),
                                           JSON_TYPE);
            });

        _jsonrpcServer->bind(ENDPOINT_VERSION,
                             (std::function<brayns::Version()>)[] {
                                 return brayns::Version();
                             });
    }

    void _handleVolumeHistogram()
    {
        _handleObjectSchema<Histogram>(ENDPOINT_VOLUME_HISTOGRAM);

        using namespace rockets::http;

        auto func = [engine = _engine](const Request&)
        {
            auto volumeHandler = engine->getScene().getVolumeHandler();
            if (!volumeHandler)
                return make_ready_response(Code::NOT_SUPPORTED);
            const auto& histo = volumeHandler->getHistogram();
            return make_ready_response(Code::OK, to_json(histo), JSON_TYPE);
        };

        _rocketsServer->handle(Method::GET, ENDPOINT_VOLUME_HISTOGRAM, func);
    }

    void _handleVolumeParams()
    {
        auto& params = _parametersManager.getVolumeParameters();
        auto postUpdate = [this](VolumeParameters&) {
            _engine->markRebuildScene();
        };
        _handleGET(ENDPOINT_VOLUME_PARAMS, params);
        _handlePUT(ENDPOINT_VOLUME_PARAMS, params, postUpdate);
    }

    void _handleSchemaRPC()
    {
        RpcDocumentation doc{"Get the schema of the given endpoint", "endpoint",
                             "name of the endpoint to get its schema"};

        _jsonrpcServer->bind(METHOD_SCHEMA, [& schemas = _schemas](
                                                const auto& request) {
            SchemaParam param;
            if (::from_json(param, request.message))
            {
                if (schemas.count(param.endpoint) == 0)
                    return Response{
                        Response::Error{"Endpoint not found", -12347}};

                auto schema = schemas[param.endpoint];
                return Response{std::move(schema)};
            }
            return rockets::jsonrpc::Response::invalidParams();
        });

        _handleSchema(
            METHOD_SCHEMA,
            buildJsonRpcSchema<SchemaParam, std::string>(METHOD_SCHEMA, doc));
    }

    void _handleInspect()
    {
        using Position = std::array<float, 2>;
        RpcDocumentation doc{"Inspect the scene at x-y position", "position",
                             "x-y position in normalized coordinates"};
        _handleRPC<Position, Renderer::PickResult>(
            METHOD_INSPECT, doc, [engine = _engine](const auto& position) {
                return engine->getRenderer().pick({position[0], position[1]});
            });
    }

    void _handleQuit()
    {
        _handleRPC(METHOD_QUIT, "Quit the application", [engine = _engine] {
            engine->setKeepRunning(false);
            engine->triggerRender();
        });
    }

    void _handleResetCamera()
    {
        _handleRPC(METHOD_RESET_CAMERA,
                   "Resets the camera to its initial values", [this] {
                       _engine->setDefaultCamera();
                       _jsonrpcServer->notify(getNotificationEndpointName(
                                                  ENDPOINT_CAMERA),
                                              _engine->getCamera());
                       _engine->triggerRender();
                   });
    }

    void _handleSnapshot()
    {
        RpcDocumentation doc{"Make a snapshot of the current view", "settings",
                             "Snapshot settings for quality and size"};
        auto func =
            [ engine = _engine,
              &imageGenerator = _imageGenerator ](auto&& params, const auto)
        {
            using SnapshotTask = DeferredTask<ImageGenerator::ImageBase64>;
            return std::make_shared<SnapshotTask>(
                SnapshotFunctor{*engine, std::move(params), imageGenerator});
        };
        _handleTask<SnapshotParams, ImageGenerator::ImageBase64>(
            METHOD_SNAPSHOT, doc, func);
    }

    void _handleStreamTo()
    {
        RpcDocumentation doc{"Stream to a displawall", "param",
                             "Stream parameters"};

        _jsonrpcServer->bind(METHOD_STREAM_TO, [
            engine = _engine, &server = _rocketsServer
        ](const auto& request) {
            auto& streamParams =
                engine->getParametersManager().getStreamParameters();
            if (::from_json(streamParams, request.message))
            {
                streamParams.markModified();
                engine->triggerRender();

                const auto& msg =
                    rockets::jsonrpc::makeNotification(METHOD_STREAM_TO,
                                                       streamParams);
                server->broadcastText(msg, {request.clientID});
                return rockets::jsonrpc::Response{"null"};
            }
            return rockets::jsonrpc::Response::invalidParams();
        });

        _handleSchema(METHOD_STREAM_TO,
                      buildJsonRpcSchema<StreamParameters>(METHOD_STREAM_TO,
                                                           doc));
    }

    void _handleRequestModelUpload()
    {
        RpcDocumentation doc{
            "Request upload of blob to trigger adding of model after blob has "
            "been received; returns model descriptor on success",
            "param", "size, type, name, transformation, etc."};

        _handleTask<BinaryParam, ModelDescriptorPtr>(
            METHOD_REQUEST_MODEL_UPLOAD, doc,
            std::bind(&BinaryRequests::createTask, std::ref(_binaryRequests),
                      std::placeholders::_1, std::placeholders::_2, _engine));
    }

    void _handleChunk()
    {
        RpcDocumentation doc{
            "Indicate sending of a binary chunk after this message", "chunk",
            "object with an ID of the chunk"};

        _handleRPC<Chunk>(METHOD_CHUNK, doc,
                          [& req = _binaryRequests](const auto& chunk) {
                              req.setNextChunkID(chunk.id);
                          });
    }

    void _handleAddModel()
    {
        RpcDocumentation doc{
            "Add model from remote path; returns model descriptor on success",
            "model_param",
            "Model parameters including name, path, transformation, etc."};

        auto func = [engine = _engine](const auto& modelParam, const auto)
        {
            return std::make_shared<AddModelTask>(modelParam, engine);
        };
        _handleTask<ModelParams, ModelDescriptorPtr>(METHOD_ADD_MODEL, doc,
                                                     func);
    }

    void _handleRemoveModel()
    {
        RpcDocumentation doc{
            "Remove the model(s) with the given ID(s) from the scene", "ids",
            "Array of model IDs"};
        _handleRPC<size_ts, bool>(METHOD_REMOVE_MODEL, doc,
                                  [engine = _engine](const size_ts& ids) {
                                      for (const auto id : ids)
                                          engine->getScene().removeModel(id);
                                      engine->triggerRender();
                                      return true;
                                  });
    }

    void _handleUpdateModel()
    {
        _jsonrpcServer->bind(
            METHOD_UPDATE_MODEL,
            [engine = _engine](const rockets::jsonrpc::Request& request) {
                ModelDescriptor newDesc;
                if (!::from_json(newDesc, request.message))
                    return Response::invalidParams();

                auto& models = engine->getScene().getModelDescriptors();
                auto i = std::find_if(models.begin(), models.end(),
                                      [id = newDesc.getModelID()](auto desc) {
                                          return id == desc->getModelID();
                                      });
                if (i == models.end())
                    return Response{to_json(false)};

                ::from_json(**i, request.message);
                engine->getScene().markModified();
                engine->triggerRender();
                return Response{to_json(true)};
            });
        RpcDocumentation doc{"Update the model with the given values", "model",
                             "Model descriptor"};
        _handleSchema(METHOD_UPDATE_MODEL,
                      buildJsonRpcSchema<ModelDescriptor, bool>(
                          METHOD_UPDATE_MODEL, doc));
    }

    void _handleGetInstances()
    {
        RpcDocumentation doc{"Get instances", "id, range",
                             "ModelID and result range"};
        _handleRPC<GetInstances, ModelInstances>(
            METHOD_GET_INSTANCES, doc,
            [engine = _engine](const GetInstances& param)->ModelInstances {
                auto id = param.modelID;
                auto& scene = engine->getScene();
                auto model = scene.getModel(id);
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           -12345);

                const auto& instances = model->getInstances();
                const Vector2ui range{std::min(param.resultRange.x(),
                                               unsigned(instances.size())),
                                      std::min(param.resultRange.y(),
                                               unsigned(instances.size()))};
                return {instances.begin() + range.x(),
                        instances.begin() + range.y()};
            });
    }

    void _handleUpdateInstance()
    {
        _jsonrpcServer->bind(
            METHOD_UPDATE_INSTANCE,
            [engine = _engine](const rockets::jsonrpc::Request& request) {
                ModelInstance newDesc;
                if (!::from_json(newDesc, request.message))
                    return Response::invalidParams();

                auto& scene = engine->getScene();
                auto model = scene.getModel(newDesc.getModelID());
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           -12345);

                auto instance = model->getInstance(newDesc.getInstanceID());
                if (!instance)
                    throw rockets::jsonrpc::response_error("Instance not found",
                                                           -12346);

                ::from_json(*instance, request.message);
                model->getModel().markInstancesDirty();
                scene.markModified();
                engine->triggerRender();
                return Response{to_json(true)};
            });
        RpcDocumentation doc{"Update the instance with the given values",
                             "model_instance", "Model instance"};
        _handleSchema(METHOD_UPDATE_INSTANCE,
                      buildJsonRpcSchema<ModelInstance, bool>(
                          METHOD_UPDATE_INSTANCE, doc));
    }

    EnginePtr _engine;

    using WsBroadcastOperations = std::map<std::string, std::function<void()>>;
    WsBroadcastOperations _wsBroadcastOperations;

    std::unordered_map<std::string, std::string> _schemas;

    ParametersManager& _parametersManager;

    std::unique_ptr<rockets::Server> _rocketsServer;
    using JsonRpcServer = rockets::jsonrpc::Server<rockets::Server>;
    std::unique_ptr<JsonRpcServer> _jsonrpcServer;

    bool _manualProcessing{true};

    ImageGenerator _imageGenerator;

    Timer _timer;
    float _leftover{0.f};

    std::map<TaskPtr, std::shared_ptr<async::task<void>>> _tasks;
    std::mutex _tasksMutex;

    BinaryRequests _binaryRequests;
};

RocketsPlugin::RocketsPlugin(EnginePtr engine, PluginAPI* api)
    : _impl{std::make_shared<Impl>(engine, api)}
{
}

void RocketsPlugin::preRender()
{
    _impl->preRender();
}

void RocketsPlugin::postRender()
{
    _impl->postRender();
}

void RocketsPlugin::postSceneLoading()
{
    _impl->postSceneLoading();
}

void RocketsPlugin::_registerRequest(const std::string& name,
                                     const RetParamFunc& action)
{
    _impl->_jsonrpcServer->bind(name, [ action, engine = _impl->_engine ](
                                          const auto& request) {
        engine->triggerRender();
        return Response{action(request.message)};
    });
}

void RocketsPlugin::_registerRequest(const std::string& name,
                                     const RetFunc& action)
{
    _impl->_jsonrpcServer->bind(name, [ action,
                                        engine = _impl->_engine ](const auto&) {
        engine->triggerRender();
        return Response{action()};
    });
}

void RocketsPlugin::_registerNotification(const std::string& name,
                                          const ParamFunc& action)
{
    _impl->_jsonrpcServer->connect(name, [ action, engine = _impl->_engine ](
                                             const auto& request) {
        engine->triggerRender();
        action(request.message);
    });
}

void RocketsPlugin::_registerNotification(const std::string& name,
                                          const VoidFunc& action)
{
    _impl->_jsonrpcServer->connect(name, [ action, engine = _impl->_engine ] {
        engine->triggerRender();
        action();
    });
}
}
