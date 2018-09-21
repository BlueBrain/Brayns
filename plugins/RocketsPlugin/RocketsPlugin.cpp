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
#include "jsonPropertyMap.h"

#include "RocketsPlugin.h"

#include <brayns/common/Timer.h>
#include <brayns/common/tasks/Task.h>
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
#include "Throttle.h"

namespace
{
constexpr int64_t INTERACTIVE_THROTTLE = 1;
constexpr int64_t DEFAULT_THROTTLE = 50;
constexpr int64_t SLOW_THROTTLE = 750;

const int MODEL_NOT_FOUND = -12345;
const int INSTANCE_NOT_FOUND = -12346;
const int TASK_RESULT_TO_JSON_ERROR = -12347;
const int SCHEMA_RPC_ENDPOINT_NOT_FOUND = -12348;
const int PARAMETER_FROM_JSON_ERROR = -12349;

// REST PUT & GET, JSONRPC set-* notification, JSONRPC get-* request
const std::string ENDPOINT_ANIMATION_PARAMS = "animation-parameters";
const std::string ENDPOINT_APP_PARAMS = "application-parameters";
const std::string ENDPOINT_CAMERA = "camera";
const std::string ENDPOINT_CAMERA_PARAMS = "camera-params";
const std::string ENDPOINT_GEOMETRY_PARAMS = "geometry-parameters";
const std::string ENDPOINT_RENDERER = "renderer";
const std::string ENDPOINT_RENDERER_PARAMS = "renderer-params";
const std::string ENDPOINT_SCENE = "scene";
const std::string ENDPOINT_SCENE_PARAMS = "scene-parameters";
const std::string ENDPOINT_STREAM = "stream";
const std::string ENDPOINT_TRANSFER_FUNCTION = "transfer-function";
const std::string ENDPOINT_VOLUME_PARAMS = "volume-parameters";

// REST GET, JSONRPC get-* request
const std::string ENDPOINT_STATISTICS = "statistics";
const std::string ENDPOINT_VERSION = "version";

// REST GET
const std::string ENDPOINT_FRAME_BUFFERS = "frame-buffers";
const std::string ENDPOINT_SIMULATION_HISTOGRAM = "simulation-histogram";

// JSONRPC async requests
const std::string METHOD_ADD_MODEL = "add-model";
const std::string METHOD_SNAPSHOT = "snapshot";
// METHOD_REQUEST_MODEL_UPLOAD from BinaryRequests.h

// JSONRPC synchronous requests
const std::string METHOD_GET_INSTANCES = "get-instances";
const std::string METHOD_GET_MODEL_PROPERTIES = "get-model-properties";
const std::string METHOD_IMAGE_JPEG = "image-jpeg";
const std::string METHOD_INSPECT = "inspect";
const std::string METHOD_MODEL_PROPERTIES_SCHEMA = "model-properties-schema";
const std::string METHOD_REMOVE_MODEL = "remove-model";
const std::string METHOD_SCHEMA = "schema";
const std::string METHOD_SET_MODEL_PROPERTIES = "set-model-properties";
const std::string METHOD_UPDATE_INSTANCE = "update-instance";
const std::string METHOD_UPDATE_MODEL = "update-model";
const std::string METHOD_ADD_CLIP_PLANE = "add-clip-plane";
const std::string METHOD_GET_CLIP_PLANES = "get-clip-planes";
const std::string METHOD_UPDATE_CLIP_PLANE = "update-clip-plane";
const std::string METHOD_REMOVE_CLIP_PLANES = "remove-clip-planes";

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
template <class T, class PRE>
bool preUpdate(const std::string& json, PRE preUpdateFunc,
               typename std::enable_if<!std::is_abstract<T>::value>::type* = 0)
{
    if (std::function<bool(const T&)>(preUpdateFunc))
    {
        T temp;
        if (!staticjson::from_json_string(json.c_str(), &temp, nullptr))
            return false;
        if (!preUpdateFunc(temp))
            return false;
    }
    return true;
}

template <class T, class PRE>
bool preUpdate(const std::string&, PRE,
               typename std::enable_if<std::is_abstract<T>::value>::type* = 0)
{
    return true;
}

template <class T, class PRE, class POST>
inline bool from_json(T& obj, const std::string& json,
                      PRE preUpdateFunc = [] {}, POST postUpdateFunc = [] {})
{
    staticjson::ParseStatus status;

    if (!preUpdate<T>(json, preUpdateFunc))
        return false;

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

#ifdef BRAYNS_USE_LIBUV
        if (uvw::Loop::getDefault()->alive())
        {
            _processDelayedNotifies =
                uvw::Loop::getDefault()->resource<uvw::AsyncHandle>();
            _processDelayedNotifies->on<uvw::AsyncEvent>(
                [&](const auto&, auto&) { this->processDelayedNotifies(); });
        }
#endif
    }

    ~Impl()
    {
        // cancel all pending tasks
        decltype(_tasks) tasksToCancel;
        {
            std::lock_guard<std::mutex> lock(_tasksMutex);
            tasksToCancel = _tasks;
        }

        for (const auto& entry : tasksToCancel)
        {
            entry.first->cancel();
            entry.second->wait();
        }

#ifdef BRAYNS_USE_LIBUV
        if (_processDelayedNotifies)
            _processDelayedNotifies->close();
#endif

        if (_rocketsServer)
            _rocketsServer->setSocketListener(nullptr);
    }

    void preRender()
    {
        if (!_rocketsServer || !_manualProcessing)
            return;

        try
        {
            processDelayedNotifies();
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

        _broadcastImageJpeg();
    }

    void processDelayedNotifies()
    {
        // call pending notifies from delayed throttle threads here as
        // notify() and process() are not threadsafe within Rockets.
        std::lock_guard<std::mutex> lock(_delayedNotifiesMutex);
        for (const auto& func : _delayedNotifies)
            func();
        _delayedNotifies.clear();
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

    void _delayedNotify(const std::function<void()>& notify)
    {
        std::lock_guard<std::mutex> lock(_delayedNotifiesMutex);
        _delayedNotifies.push_back(notify);

#ifdef BRAYNS_USE_LIBUV
        if (_processDelayedNotifies)
        {
            // dispatch delayed notify from throttle thread to main thread
            // (where the default loop runs) as notify() and process() are not
            // threadsafe within Rockets.
            _processDelayedNotifies->send();
        }
#endif
    }

    void _rebroadcast(const std::string& endpoint,
                      const std::string& message,
                      const std::set<uintptr_t>& filter)
    {
        _delayedNotify([&, message, filter] {
            if (_rocketsServer->getConnectionCount() > 1)
            {
                const auto& msg =
                    rockets::jsonrpc::makeNotification(endpoint, message);
                _rocketsServer->broadcastText(msg, filter);
            }
        });
    }

    // Utilty to change current client while we are handling a message to skip
    // notification to the current client and to trigger a delayed notify to
    // avoid a deadlock which happens when sending a message from within a
    // message handler.
    struct ScopedCurrentClient
    {
        ScopedCurrentClient(uintptr_t& currentClientID, const uintptr_t newID)
            : _currentClientID(currentClientID)
        {
            _currentClientID = newID;
        }

        ~ScopedCurrentClient() { _currentClientID = NO_CURRENT_CLIENT; }
    private:
        uintptr_t& _currentClientID;
    };

    void _bindEndpoint(const std::string& method,
                       rockets::jsonrpc::ResponseCallback action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);
            return action(request);
        });
    }

    template <typename Params, typename RetVal>
    void _bindEndpoint(const std::string& method,
                       std::function<RetVal(Params)> action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            Params params;
            if (!::from_json(params, request.message))
                return Response::invalidParams();
            try
            {
                const auto& ret = action(std::move(params));
                return Response{to_json(ret)};
            }
            catch (const rockets::jsonrpc::response_error& e)
            {
                return Response{Response::Error{e.what(), e.code}};
            }
        });
    }

    template <typename RetVal>
    void _bindEndpoint(const std::string& method,
                       std::function<RetVal()> action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            try
            {
                const auto& ret = action();
                return Response{to_json(ret)};
            }
            catch (const rockets::jsonrpc::response_error& e)
            {
                return Response{Response::Error{e.what(), e.code}};
            }
        });
    }

    template <typename Params>
    void _bindEndpoint(const std::string& method,
                       std::function<void(Params)> action)
    {
        _jsonrpcServer->bind(method, [&, action](
                                         rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);

            Params params;
            if (!::from_json(params, request.message))
                return Response::invalidParams();
            action(std::move(params));
            return Response{"\"OK\""};
        });
    }

    void _bindEndpoint(const std::string& method,
                       rockets::jsonrpc::VoidCallback action)
    {
        _jsonrpcServer->connect(method, [&, action](
                                            rockets::jsonrpc::Request request) {
            ScopedCurrentClient scope(_currentClientID, request.clientID);
            action();
            return Response{"\"OK\""};
        });
    }

    template <class T>
    void _handleGET(const std::string& endpoint, T& obj,
                    const int64_t throttleTime = DEFAULT_THROTTLE)
    {
        using namespace rockets::http;

        _rocketsServer->handle(Method::GET, endpoint, [&obj](const Request&) {
            return make_ready_response(Code::OK, to_json(obj), JSON_TYPE);
        });

        _handleObjectSchema(endpoint, obj);

        const std::string rpcEndpoint = getRequestEndpointName(endpoint);

        _jsonrpcServer->bind(rpcEndpoint,
                             std::function<const T&()>(
                                 [&obj]() -> const T& { return obj; }));
        _handleSchema(rpcEndpoint,
                      buildJsonRpcSchemaRequestReturnOnly(
                          {rpcEndpoint, "Get the current state of " + endpoint},
                          obj));

        // Create new throttle for that endpoint
        _throttle[endpoint];

        obj.onModified(
            [&, endpoint=getNotificationEndpointName(endpoint), throttleTime](const auto& base) {
                auto& throttle = _throttle[endpoint];

                // throttle itself is not thread-safe, but we can get called
                // from different threads (c.f. async model load)
                std::lock_guard<std::mutex> lock(throttle.first);

                const auto& castedObj = static_cast<const T&>(base);
                const auto notify = [&rocketsServer=_rocketsServer,
                                     clientID=_currentClientID, endpoint,
                                     json=to_json(castedObj)]
                {
                    const auto& msg =
                        rockets::jsonrpc::makeNotification(endpoint, json);
                    if(clientID == NO_CURRENT_CLIENT)
                        rocketsServer->broadcastText(msg);
                    else
                        rocketsServer->broadcastText(msg, {clientID});
                };
                const auto delayedNotify = [&, notify]{
                    this->_delayedNotify(notify);
                };

                // non-throttled, direct notify can happen directly if we are
                // not in the middle handling an incoming message; delayed
                // notify must be dispatched to the main thread
                if(_currentClientID == NO_CURRENT_CLIENT)
                    throttle.second(notify, delayedNotify, throttleTime);
                else
                    throttle.second(delayedNotify, delayedNotify, throttleTime);
            });
    }

    template <class T>
    void _handlePUT(const std::string& endpoint, T& obj)
    {
        _handlePUT(endpoint, obj, std::function<bool(const T&)>(),
                   std::function<void(T&)>());
    }

    template <class T, class PRE, class POST>
    void _handlePUT(const std::string& endpoint, T& obj, PRE preUpdateFunc,
                    POST postUpdateFunc)
    {
        using namespace rockets::http;
        _rocketsServer->handle(Method::PUT, endpoint, [&obj, preUpdateFunc,
                                                       postUpdateFunc](
                                                          const Request& req) {
            return make_ready_response(
                from_json(obj, req.body, preUpdateFunc, postUpdateFunc)
                    ? Code::OK
                    : Code::BAD_REQUEST);
        });

        _handleObjectSchema(endpoint, obj);

        const std::string rpcEndpoint = getNotificationEndpointName(endpoint);

        _bindEndpoint(rpcEndpoint, [&, rpcEndpoint, preUpdateFunc,
                                    postUpdateFunc](
                                       rockets::jsonrpc::Request request) {
            if (from_json(obj, request.message, preUpdateFunc, postUpdateFunc))
            {
                _engine->triggerRender();
                return rockets::jsonrpc::Response{to_json(true)};
            }
            return rockets::jsonrpc::Response::invalidParams();
        });
        const RpcParameterDescription desc{rpcEndpoint,
                                           "Set the new state of " + endpoint,
                                           "param", endpoint};
        _handleSchema(rpcEndpoint,
                      buildJsonRpcSchemaRequest<T, bool>(desc, obj));
    }

    template <class T>
    void _handle(const std::string& endpoint, T& obj,
                 const int64_t throttleTime = DEFAULT_THROTTLE)
    {
        _handleGET(endpoint, obj, throttleTime);
        _handlePUT(endpoint, obj);
    }

    template <class P, class R>
    void _handleRPC(const RpcParameterDescription& desc,
                    std::function<R(P)> action)
    {
        _bindEndpoint<P, R>(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaRequest<P, R>(desc));
    }

    template <class R>
    void _handleRPC(const RpcDescription& desc, std::function<R()> action)
    {
        _bindEndpoint<R>(desc.methodName, action);
        _handleSchema(desc.methodName,
                      buildJsonRpcSchemaRequestReturnOnly<R>(desc));
    }

    template <class P>
    void _handleRPC(const RpcParameterDescription& desc,
                    std::function<void(P)> action)
    {
        _bindEndpoint<P>(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaNotify<P>(desc));
    }

    void _handleRPC(const RpcDescription& desc, std::function<void()> action)
    {
        _bindEndpoint(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaNotify(desc));
    }

    template <class P, class R>
    void _handleAsyncRPC(const RpcParameterDescription& desc,
                         std::function<rockets::jsonrpc::CancelRequestCallback(
                             P, uintptr_t, rockets::jsonrpc::AsyncResponse,
                             rockets::jsonrpc::ProgressUpdateCallback)>
                             action)
    {
        _jsonrpcServer->bindAsync<P>(desc.methodName, action);
        _handleSchema(desc.methodName, buildJsonRpcSchemaRequest<P, R>(desc));
    }

    template <class P, class R>
    void _handleTask(
        const RpcParameterDescription& desc,
        std::function<std::shared_ptr<Task<R>>(P, uintptr_t)> createTask)
    {
        // define the action that is executed on every incoming request from the
        // client:
        // - create the task that shall be executed
        // - wire the result of task to the response callback from rockets
        // - setup progress reporting during the task execution using libuv
        // - wire the cancel request from rockets to the task

        auto action = [&, createTask](P params, auto clientID, auto respond,
                                      auto progressCb) {
            // transform task error to rockets error response
            auto errorCallback = [&, respond](const TaskRuntimeError& error) {
                const Response response(
                    Response::Error{error.what(), error.code, error.data});
                std::lock_guard<std::mutex> lock(_delayedNotifiesMutex);
                _delayedNotifies.push_back(
                    [respond, response] { respond(response); });
            };

            try
            {
                // transform task result to rockets response
                auto readyCallback = [&, respond](const R& result) {
                    std::lock_guard<std::mutex> lock(_delayedNotifiesMutex);
                    try
                    {
                        _delayedNotifies.push_back(
                            [respond, result] { respond({to_json(result)}); });
                    }
                    catch (const std::runtime_error& e)
                    {
                        const Response response(
                            Response::Error{e.what(),
                                            TASK_RESULT_TO_JSON_ERROR});
                        _delayedNotifies.push_back(
                            [respond, response] { respond(response); });
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

                    using ms = std::chrono::milliseconds;
                    progressUpdate->start(ms(0), ms(SLOW_THROTTLE));
                }
#endif

                // setup the continuation task that handles the result or error
                // of the task to handle the responses to rockets accordingly.
                auto responseTask =
                    std::make_shared<async::task<void>>(task->get().then(
                        [&, readyCallback, errorCallback, task,
                         finishProgress](typename Task<R>::Type result) {
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

#ifdef BRAYNS_USE_LIBUV
                            if (_processDelayedNotifies)
                                _processDelayedNotifies->send();
#endif

                            std::lock_guard<std::mutex> lock(_tasksMutex);
                            _tasks.erase(task);
                            _binaryRequests.removeTask(task);
                        }));

                std::lock_guard<std::mutex> lock(_tasksMutex);
                _tasks.emplace(task, responseTask);

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
        _handleAsyncRPC<P, R>(desc, action);
    }

    template <class T>
    void _handleObjectSchema(const std::string& endpoint)
    {
        _handleSchema(endpoint,
                      buildJsonSchema<T>(hyphenatedToCamelCase(endpoint)));
    }

    template <class T>
    void _handleObjectSchema(const std::string& endpoint, T& obj)
    {
        _handleSchema(endpoint,
                      buildJsonSchema(obj, hyphenatedToCamelCase(endpoint)));
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
        _handleCamera();
        _handleImageJPEG();
        _handleRenderer();
        _handleStreaming();
        _handleVersion();

        _handle(ENDPOINT_APP_PARAMS,
                _parametersManager.getApplicationParameters());
        _handle(ENDPOINT_ANIMATION_PARAMS,
                _parametersManager.getAnimationParameters(),
                INTERACTIVE_THROTTLE);
        _handle(ENDPOINT_GEOMETRY_PARAMS,
                _parametersManager.getGeometryParameters());
        _handle(ENDPOINT_SCENE_PARAMS, _parametersManager.getSceneParameters());
        _handle(ENDPOINT_VOLUME_PARAMS,
                _parametersManager.getVolumeParameters());

        // following endpoints need a valid engine
        _handle(ENDPOINT_TRANSFER_FUNCTION,
                _engine->getScene().getTransferFunction());
        _handle(ENDPOINT_SCENE, _engine->getScene());

        _handleGET(ENDPOINT_STATISTICS, _engine->getStatistics(),
                   SLOW_THROTTLE);

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
        _handleSetModelProperties();
        _handleGetModelProperties();
        _handleModelPropertiesSchema();

        _handleAddClipPlane();
        _handleGetClipPlanes();
        _handleUpdateClipPlane();
        _handleRemoveClipPlanes();

        _handleGetInstances();
        _handleUpdateInstance();

        _handlePropertyObject(_engine->getCamera(), ENDPOINT_CAMERA_PARAMS,
                              "camera");
        _handlePropertyObject(_engine->getRenderer(), ENDPOINT_RENDERER_PARAMS,
                              "renderer");
    }

    void _handleFrameBuffer()
    {
        // don't add framebuffer to websockets for performance
        using namespace rockets::http;
        _rocketsServer->handleGET(ENDPOINT_FRAME_BUFFERS,
                                  _engine->getFrameBuffer());
        _handleObjectSchema(ENDPOINT_FRAME_BUFFERS, _engine->getFrameBuffer());
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
        _handleSchema(
            METHOD_IMAGE_JPEG,
            buildJsonRpcSchemaRequestReturnOnly<ImageGenerator::ImageBase64>(
                {METHOD_IMAGE_JPEG,
                 "Get the current state of " + METHOD_IMAGE_JPEG}));
    }

    void _broadcastImageJpeg()
    {
        auto& frameBuffer = _engine->getFrameBuffer();
        if (!frameBuffer.isModified())
            return;

        const auto& params = _parametersManager.getApplicationParameters();
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
            _imageGenerator.createJPEG(frameBuffer,
                                       params.getJpegCompression());
        if (image.size > 0)
            _rocketsServer->broadcastBinary((const char*)image.data.get(),
                                            image.size);
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

        _jsonrpcServer->bind(getRequestEndpointName(ENDPOINT_VERSION),
                             (std::function<brayns::Version()>)[] {
                                 return brayns::Version();
                             });

        _handleSchema(ENDPOINT_VERSION, version.getSchema());
    }

    void _handleCamera()
    {
        auto& camera = _engine->getCamera();
        auto preUpdate = [types = camera.getTypes()](const Camera& obj)
        {
            if (obj.getCurrentType().empty())
                return true;
            return std::find(types.begin(), types.end(),
                             obj.getCurrentType()) != types.end();
        };
        _handleGET(ENDPOINT_CAMERA, camera);
        _handlePUT(ENDPOINT_CAMERA, camera, preUpdate,
                   std::function<void(Camera&)>());
    }

    void _handleRenderer()
    {
        auto& params = _parametersManager.getRenderingParameters();
        auto preUpdate = [](const auto& rp) {
            return std::find(rp.getRenderers().begin(), rp.getRenderers().end(),
                             rp.getCurrentRenderer()) !=
                   rp.getRenderers().end();
        };
        auto postUpdate = [& renderer = _engine->getRenderer()](auto& rp)
        {
            renderer.setCurrentType(rp.getCurrentRenderer());
        };
        _handleGET(ENDPOINT_RENDERER, params);
        _handlePUT(ENDPOINT_RENDERER, params, preUpdate, postUpdate);
    }

    void _handleSchemaRPC()
    {
        const RpcParameterDescription desc{
            METHOD_SCHEMA, "Get the schema of the given endpoint", "endpoint",
            "name of the endpoint to get its schema"};

        _jsonrpcServer->bind(METHOD_SCHEMA, [& schemas = _schemas](
                                                const auto& request) {
            SchemaParam param;
            if (::from_json(param, request.message))
            {
                if (schemas.count(param.endpoint) == 0)
                    return Response{
                        Response::Error{"Endpoint not found",
                                        SCHEMA_RPC_ENDPOINT_NOT_FOUND}};

                auto schema = schemas[param.endpoint];
                return Response{std::move(schema)};
            }
            return Response::invalidParams();
        });

        _handleSchema(METHOD_SCHEMA,
                      buildJsonRpcSchemaRequest<SchemaParam, std::string>(
                          desc));
    }

    void _handleInspect()
    {
        using Position = std::array<double, 2>;
        const RpcParameterDescription desc{
            METHOD_INSPECT, "Inspect the scene at x-y position", "position",
            "x-y position in normalized coordinates"};
        _handleRPC<Position, Renderer::PickResult>(
            desc, [engine = _engine](const auto& position) {
                return engine->getRenderer().pick(
                    {float(position[0]), float(position[1])});
            });
    }

    void _handleQuit()
    {
        _handleRPC({METHOD_QUIT, "Quit the application"},
                   [engine = _engine] {
                       engine->setKeepRunning(false);
                       engine->triggerRender();
                   });
    }

    void _handleResetCamera()
    {
        _handleRPC({METHOD_RESET_CAMERA,
                    "Resets the camera to its initial values"},
                   [this] {
                       _engine->setDefaultCamera();
                       _engine->triggerRender();
                   });
    }

    void _handleSnapshot()
    {
        const RpcParameterDescription desc{
            METHOD_SNAPSHOT, "Make a snapshot of the current view", "settings",
            "Snapshot settings for quality and size"};
        auto func =
            [ engine = _engine,
              &imageGenerator = _imageGenerator ](auto&& params, const auto)
        {
            using SnapshotTask = DeferredTask<ImageGenerator::ImageBase64>;
            return std::make_shared<SnapshotTask>(
                SnapshotFunctor{*engine, std::move(params), imageGenerator});
        };
        _handleTask<SnapshotParams, ImageGenerator::ImageBase64>(desc, func);
    }

    void _handleStreamTo()
    {
        const RpcParameterDescription desc{METHOD_STREAM_TO,
                                           "Stream to a displaywall", "param",
                                           "Stream parameters"};

        _bindEndpoint(METHOD_STREAM_TO, [&](const auto& request) {
            auto& streamParams =
                _engine->getParametersManager().getStreamParameters();
            if (::from_json(streamParams, request.message))
            {
                streamParams.markModified();
                _engine->triggerRender();
                return Response{to_json(true)};
            }
            return Response::invalidParams();
        });

        _handleSchema(METHOD_STREAM_TO,
                      buildJsonRpcSchemaRequest<StreamParameters, bool>(desc));
    }

    void _handleRequestModelUpload()
    {
        const RpcParameterDescription desc{
            METHOD_REQUEST_MODEL_UPLOAD,
            "Request upload of blob to trigger adding of model after blob has "
            "been received; returns model descriptor on success",
            "param", "size, type, name, transformation, etc."};

        _handleTask<BinaryParam, ModelDescriptorPtr>(
            desc,
            std::bind(&BinaryRequests::createTask, std::ref(_binaryRequests),
                      std::placeholders::_1, std::placeholders::_2, _engine));
    }

    void _handleChunk()
    {
        const RpcParameterDescription desc{
            METHOD_CHUNK,
            "Indicate sending of a binary chunk after this message", "chunk",
            "object with an ID of the chunk"};

        _handleRPC<Chunk>(desc, [& req = _binaryRequests](const auto& chunk) {
            req.setNextChunkID(chunk.id);
        });
    }

    void _handleAddClipPlane()
    {
        const RpcParameterDescription desc{
            METHOD_ADD_CLIP_PLANE,
            "Add a clip plane; returns the clip plane descriptor", "plane",
            "An array of 4 floats"};

        _handleRPC<Plane, ClipPlanePtr>(desc, [&](const Plane& plane) {
            auto& scene = _engine->getScene();
            auto clipPlane = scene.getClipPlane(scene.addClipPlane(plane));
            _rebroadcast(METHOD_UPDATE_CLIP_PLANE, to_json(clipPlane),
                         {_currentClientID});
            return clipPlane;
        });
    }

    void _handleGetClipPlanes()
    {
        _handleRPC<ClipPlanes>({METHOD_GET_CLIP_PLANES, "Get all clip planes"},
                               [engine = _engine]() {
                                   auto& scene = engine->getScene();
                                   return scene.getClipPlanes();
                               });
    }

    void _handleUpdateClipPlane()
    {
        const RpcParameterDescription desc{
            METHOD_UPDATE_CLIP_PLANE,
            "Update a clip plane with the given coefficients", "clip_plane",
            "Plane id and equation"};
        _handleRPC<ClipPlane, bool>(desc, [&](const ClipPlane& newPlane) {
            auto& scene = _engine->getScene();
            if (auto plane = scene.getClipPlane(newPlane.getID()))
            {
                plane->setPlane(newPlane.getPlane());
                _rebroadcast(METHOD_UPDATE_CLIP_PLANE, to_json(newPlane),
                             {_currentClientID});
                _engine->triggerRender();
                return true;
            }
            return false;
        });
    }

    void _handleRemoveClipPlanes()
    {
        const RpcParameterDescription desc{
            METHOD_REMOVE_CLIP_PLANES,
            "Remove clip planes from the scene given their gids", "ids",
            "Array of clip planes IDs"};
        _handleRPC<size_ts, bool>(desc, [&](const size_ts& ids) {
            for (const auto id : ids)
                _engine->getScene().removeClipPlane(id);
            _rebroadcast(METHOD_REMOVE_CLIP_PLANES, to_json(ids),
                         {_currentClientID});
            _engine->triggerRender();
            return true;
        });
    }

    void _handleAddModel()
    {
        const RpcParameterDescription desc{
            METHOD_ADD_MODEL,
            "Add model from remote path; returns model descriptor on success",
            "model_param",
            "Model parameters including name, path, transformation, etc."};

        auto func = [engine = _engine](const auto& modelParam, const auto)
        {
            return std::make_shared<AddModelTask>(modelParam, engine);
        };
        _handleTask<ModelParams, ModelDescriptorPtr>(desc, func);
    }

    void _handleRemoveModel()
    {
        const RpcParameterDescription desc{
            METHOD_REMOVE_MODEL,
            "Remove the model(s) with the given ID(s) from the scene", "ids",
            "Array of model IDs"};
        _handleRPC<size_ts, bool>(desc, [engine = _engine](const size_ts& ids) {
            for (const auto id : ids)
                engine->getScene().removeModel(id);
            engine->triggerRender();
            return true;
        });
    }

    void _handleUpdateModel()
    {
        _bindEndpoint(METHOD_UPDATE_MODEL,
                      [engine =
                           _engine](const rockets::jsonrpc::Request& request) {
                          ModelDescriptor newDesc;
                          if (!::from_json(newDesc, request.message))
                              return Response::invalidParams();

                          auto& scene = engine->getScene();
                          if (auto model = scene.getModel(newDesc.getModelID()))
                          {
                              ::from_json(*model, request.message);
                              scene.markModified();
                              engine->triggerRender();
                              return Response{to_json(true)};
                          }
                          return Response{to_json(false)};
                      });
        const RpcParameterDescription desc{
            METHOD_UPDATE_MODEL, "Update the model with the given values",
            "model", "Model descriptor"};
        _handleSchema(METHOD_UPDATE_MODEL,
                      buildJsonRpcSchemaRequest<ModelDescriptor, bool>(desc));
    }

    void _handleGetModelProperties()
    {
        const RpcParameterDescription desc{
            METHOD_GET_MODEL_PROPERTIES,
            "Get the properties of the given model", "id", "the model ID"};

        _jsonrpcServer->bind<ObjectID, PropertyMap>(
            desc.methodName, [engine = _engine](const ObjectID& id) {
                auto model = engine->getScene().getModel(id.id);
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);
                return model->getProperties();
            });

        _handleSchema(METHOD_GET_MODEL_PROPERTIES,
                      buildJsonRpcSchemaRequest<ObjectID, PropertyMap>(desc));
    }

    void _handleSetModelProperties()
    {
        const RpcParameterDescription desc{
            METHOD_SET_MODEL_PROPERTIES,
            "Set the properties of the given model", "param",
            "model ID and its properties"};

        _bindEndpoint(METHOD_SET_MODEL_PROPERTIES, [&](const auto& request) {
            using namespace rapidjson;
            Document document;
            document.Parse(request.message.c_str());

            if (!document.HasMember("id") || !document.HasMember("properties"))
            {
                return Response::invalidParams();
            }

            const auto modelID = document["id"].GetInt();
            auto model = _engine->getScene().getModel(modelID);
            if (!model)
            {
                return Response{
                    Response::Error{"Model not found", MODEL_NOT_FOUND}};
            }

            Document propertyDoc;
            propertyDoc.SetObject() = document["properties"].GetObject();
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            propertyDoc.Accept(writer);

            auto props = model->getProperties();
            if (::from_json(props, buffer.GetString()))
            {
                model->setProperties(props);
                _engine->triggerRender();

                this->_rebroadcast(METHOD_SET_MODEL_PROPERTIES,
                                   request.message, {request.clientID});

                return Response{to_json(true)};
            }
            return Response::invalidParams();
        });

        _handleSchema(METHOD_SET_MODEL_PROPERTIES,
                      buildJsonRpcSchemaRequest<ModelProperties, bool>(desc));
    }

    void _handleModelPropertiesSchema()
    {
        const RpcParameterDescription desc{
            METHOD_MODEL_PROPERTIES_SCHEMA,
            "Get the property schema of the model", "id",
            "ID of the model get its properties schema"};

        _jsonrpcServer->bind(
            METHOD_MODEL_PROPERTIES_SCHEMA,
            [engine = _engine](const auto& request) {
                ObjectID modelID;
                if (::from_json(modelID, request.message))
                {
                    auto model = engine->getScene().getModel(modelID.id);
                    if (!model)
                        return Response{Response::Error{"Model not found",
                                                        MODEL_NOT_FOUND}};

                    return Response{buildJsonSchema(model->getProperties(),
                                                    "ModelProperties")};
                }
                return Response::invalidParams();
            });

        _handleSchema(METHOD_MODEL_PROPERTIES_SCHEMA,
                      buildJsonRpcSchemaRequest<ObjectID, std::string>(desc));
    }

    void _handleGetInstances()
    {
        const RpcParameterDescription desc{METHOD_GET_INSTANCES,
                                           "Get instances", "id, range",
                                           "Model id and result range"};
        _handleRPC<GetInstances, ModelInstances>(
            desc,
            [engine = _engine](const GetInstances& param)->ModelInstances {
                auto id = param.modelID;
                auto& scene = engine->getScene();
                auto model = scene.getModel(id);
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);

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
        _bindEndpoint(
            METHOD_UPDATE_INSTANCE,
            [&](const rockets::jsonrpc::Request& request) {
                ModelInstance newDesc;
                if (!::from_json(newDesc, request.message))
                    return Response::invalidParams();

                auto& scene = _engine->getScene();
                auto model = scene.getModel(newDesc.getModelID());
                if (!model)
                    throw rockets::jsonrpc::response_error("Model not found",
                                                           MODEL_NOT_FOUND);

                auto instance = model->getInstance(newDesc.getInstanceID());
                if (!instance)
                    throw rockets::jsonrpc::response_error("Instance not found",
                                                           INSTANCE_NOT_FOUND);

                ::from_json(*instance, request.message);
                model->getModel().markInstancesDirty();
                scene.markModified(false);

                _engine->triggerRender();
                _rebroadcast(METHOD_UPDATE_INSTANCE, request.message,
                             {request.clientID});

                return Response{to_json(true)};
            });
        const RpcParameterDescription desc{
            METHOD_UPDATE_INSTANCE, "Update the instance with the given values",
            "model_instance", "Model instance"};
        _handleSchema(METHOD_UPDATE_INSTANCE,
                      buildJsonRpcSchemaRequest<ModelInstance, bool>(desc));
    }

    void _handlePropertyObject(PropertyObject& object,
                               const std::string& endpoint,
                               const std::string objectName)
    {
        const auto requestEndpoint = getRequestEndpointName(endpoint);
        const auto notifyEndpoint = getNotificationEndpointName(endpoint);

        _jsonrpcServer->bind<PropertyMap>(requestEndpoint, [& object = object] {
            return object.getPropertyMap();
        });

        _bindEndpoint(notifyEndpoint, [&, notifyEndpoint](const auto& request) {
            PropertyMap props = object.getPropertyMap();
            if (::from_json(props, request.message))
            {
                object.updateProperties(props);
                _engine->triggerRender();
                return Response{to_json(true)};
            }
            return Response::invalidParams();
        });

        std::vector<std::pair<std::string, PropertyMap>> props;
        for (const auto& type : object.getTypes())
            props.push_back(std::make_pair(type, object.getPropertyMap(type)));

        // get-<object>-params RPC schema
        _handleSchema(requestEndpoint,
                      buildJsonRpcSchemaRequestPropertyMaps(
                          {requestEndpoint,
                           "Get the params of the current " + objectName},
                          props));

        // set-<object>-params RPC schema
        const RpcParameterDescription desc{notifyEndpoint,
                                           "Set the params on the current " +
                                               objectName,
                                           "params",
                                           "new " + objectName + " params"};
        _handleSchema(notifyEndpoint,
                      buildJsonRpcSchemaNotifyPropertyMaps(desc, props));

        // <object>-params object schema
        _handleSchema(endpoint,
                      buildJsonSchema(props, hyphenatedToCamelCase(endpoint)));
    }

    EnginePtr _engine;

    std::unordered_map<std::string, std::pair<std::mutex, Throttle>> _throttle;
    std::vector<std::function<void()>> _delayedNotifies;
    std::mutex _delayedNotifiesMutex;
    static constexpr uintptr_t NO_CURRENT_CLIENT{0};
    uintptr_t _currentClientID{NO_CURRENT_CLIENT};

#ifdef BRAYNS_USE_LIBUV
    std::shared_ptr<uvw::AsyncHandle> _processDelayedNotifies;
#endif

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

void RocketsPlugin::registerNotification(
    const RpcParameterDescription& desc, const PropertyMap& input,
    const std::function<void(PropertyMap)>& action)
{
    _impl->_jsonrpcServer->connect(desc.methodName, [
        name = desc.methodName, input, action, engine = _impl->_engine
    ](const auto& request) {
        PropertyMap params = input;
        if (::from_json(params, request.message))
        {
            action(params);
            engine->triggerRender();
            return;
        }
        BRAYNS_ERROR << "from_json for " << name << " failed" << std::endl;
    });

    _impl->_handleSchema(desc.methodName,
                         buildJsonRpcSchemaNotifyPropertyMap(desc, input));
}

void RocketsPlugin::registerNotification(const RpcDescription& desc,
                                         const std::function<void()>& action)
{
    _impl->_jsonrpcServer->connect(desc.methodName,
                                   [ action, engine = _impl->_engine ] {
                                       action();
                                       engine->triggerRender();
                                   });

    _impl->_handleSchema(desc.methodName, buildJsonRpcSchemaNotify(desc));
}

void RocketsPlugin::registerRequest(
    const RpcParameterDescription& desc, const PropertyMap& input,
    const PropertyMap& output,
    const std::function<PropertyMap(PropertyMap)>& action)
{
    _impl->_bindEndpoint(desc.methodName, [
        name = desc.methodName, input, action, engine = _impl->_engine
    ](const auto& request) {
        PropertyMap params = input;
        if (::from_json(params, request.message))
        {
            engine->triggerRender();
            return Response{to_json(action(params))};
        }
        return Response{Response::Error{"from_json for " + name + " failed",
                                        PARAMETER_FROM_JSON_ERROR}};
    });

    _impl->_handleSchema(desc.methodName,
                         buildJsonRpcSchemaRequestPropertyMap(desc, input,
                                                              output));
}

void RocketsPlugin::registerRequest(const RpcDescription& desc,
                                    const PropertyMap& output,
                                    const std::function<PropertyMap()>& action)
{
    _impl->_jsonrpcServer->bind(desc.methodName,
                                [ action,
                                  engine = _impl->_engine ](const auto&) {
                                    engine->triggerRender();
                                    return Response{to_json(action())};
                                });

    _impl->_handleSchema(desc.methodName,
                         buildJsonRpcSchemaRequestPropertyMap(desc, output));
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
        action(request.message);
        engine->triggerRender();
    });
}

void RocketsPlugin::_registerNotification(const std::string& name,
                                          const VoidFunc& action)
{
    _impl->_jsonrpcServer->connect(name, [ action, engine = _impl->_engine ] {
        action();
        engine->triggerRender();
    });
}
}
