/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "FluorescencePlugin.h"
#include "Log.h"

#include <brayns/common/ActionInterface.h>
#include <brayns/common/PropertyMap.h>

#include <brayns/engine/Engine.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/pluginapi/PluginAPI.h>

#define HANDLE_NOT_PARSED(req, resp) \
    if(!req.parsed) { \
        resp.error = 1; \
        resp.message = req.parseMessage; \
        return resp; \
    } \

template<typename Request>
inline bool handleCommonSensorParams(const Request& req, RequestResponse& resp)
{
    if(req.rotation.size() < 4)
    {
        resp.error = 2;
        resp.message = "Rotation param requires 4 float numbers (angle + axis)";
        return false;
    }

    if(req.translation.size() < 3)
    {
        resp.error = 3;
        resp.message = "Translation param requires 3 float numbers";
        return false;
    }

    if(req.scale.size() < 3)
    {
        resp.error = 4;
        resp.message = "Scale param requires 3 float numbers";
        return false;
    }

    return true;
}

template<typename Request>
inline bool handleCommonVolumeParams(const Request& req, RequestResponse& resp)
{
    if(req.p0.size() < 3)
    {
        resp.error = 2;
        resp.message = "Volume minimun bound (p0) requires 3 float values";
        return false;
    }

    if(req.p1.size() < 3)
    {
        resp.error = 3;
        resp.message = "Volume maximun bound (p1) requires 3 float values";
        return false;
    }

    return true;
}

template<typename Request>
inline void addCommonSensorParams(const Request& req, brayns::PropertyMap& metaObject)
{
    const std::array<double, 4> rotation = {req.rotation[0],
                                            req.rotation[1],
                                            req.rotation[2],
                                            req.rotation[3]};
    metaObject.setProperty({"sensor_rotation", rotation});

    const std::array<double, 3> translation = {req.translation[0],
                                               req.translation[1],
                                               req.translation[2]};
    metaObject.setProperty({"sensor_translation", translation});

    const std::array<double, 3> scale = {req.scale[0],
                                         req.scale[1],
                                         req.scale[2]};
    metaObject.setProperty({"sensor_scale", scale});

    metaObject.setProperty({"reference", req.reference});
    metaObject.setProperty({"xpixels", req.xpixels});
    metaObject.setProperty({"ypixels", req.ypixels});
    metaObject.setProperty({"fov", static_cast<double>(req.fov)});
}

template<typename Request>
inline void addCommonVolumeParams(const Request& req, brayns::PropertyMap& metaObject)
{
    const std::array<double, 3> p0 = {req.p0[0], req.p0[1], req.p1[2]};
    metaObject.setProperty({"p0", p0});
    const std::array<double, 3> p1 = {req.p1[0], req.p1[1], req.p1[2]};
    metaObject.setProperty({"p1", p1});
}

FluorescencePlugin::FluorescencePlugin()
    : ExtensionPlugin()
{
}

FluorescencePlugin::~FluorescencePlugin()
{
}

void FluorescencePlugin::init()
{
    auto actionInterface = _api->getActionInterface();
    if(actionInterface)
    {
        PLUGIN_INFO << "Registering 'add-rectangle-sensor' endpoint" << std::endl;
        actionInterface->registerRequest<AddRectangleSensorRequest, RequestResponse>(
            "add-rectangle-sensor", [&](const AddRectangleSensorRequest& request){
                return _addRectangleSensor(request);
        });

        PLUGIN_INFO << "Registering 'add-disk-sensor' endpoint" << std::endl;
        actionInterface->registerRequest<AddDiskSensorRequest, RequestResponse>(
            "add-disk-sensor", [&](const AddDiskSensorRequest& request){
                return _addDiskSensor(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentVolume, RequestResponse>(
            "add-fluorescent-volume", [&](const AddFluorescentVolume& request){
                return _addFluorescentVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-ann-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentAnnotatedVolume, RequestResponse>(
            "add-fluorescent-ann-volume", [&](const AddFluorescentAnnotatedVolume& request){
                return _addFluorescentAnnotatedVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-bin-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentBinaryVolume, RequestResponse>(
            "add-fluorescent-bin-volume", [&](const AddFluorescentBinaryVolume& request){
                return _addFluorescentBinaryVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-grid-file-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentGridFromFileVolume, RequestResponse>(
            "add-fluorescent-grid-file-volume",
                    [&](const AddFluorescentGridFromFileVolume& request){
                return _addFluorescentGridVolumeFromFile(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-grid-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentGridVolume, RequestResponse>(
            "add-fluorescent-grid-volume", [&](const AddFluorescentGridVolume& request){
                return _addFluorescentGridVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-scatter-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentScatteringVolume, RequestResponse>(
            "add-fluorescent-scatter-volume", [&](const AddFluorescentScatteringVolume& request){
                return _addFluorescentScatteringVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-scattergrid-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddFluorescentScatteringGridVolume, RequestResponse>(
            "add-fluorescent-scattergrid-volume",
                    [&](const AddFluorescentScatteringGridVolume& request){
                return _addFluorescentScatteringGridVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-fluorescent-scattergridfile-volume' endpoint"
                    << std::endl;
        actionInterface->
                registerRequest<AddFluorescentScatteringGridFromFileVolume, RequestResponse>(
            "add-fluorescent-scattergridfile-volume",
                    [&](const AddFluorescentScatteringGridFromFileVolume& request){
                return _addFluorescentScatteringGridVolumeFromFile(request);
        });
    }
}

RequestResponse
FluorescencePlugin::_addRectangleSensor(const AddRectangleSensorRequest& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp)

    if(!handleCommonSensorParams(req, resp))
        return resp;

    brayns::PropertyMap metaObject;

    metaObject.setProperty({"sensor_shape_type", std::string("rectangle")});

    addCommonSensorParams(req, metaObject);

    metaObject.setProperty({"sensor_shape_height", static_cast<double>(req.height)});
    metaObject.setProperty({"sensor_shape_x", static_cast<double>(req.x)});
    metaObject.setProperty({"sensor_shape_y", static_cast<double>(req.y)});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.reference.empty()? "sensor" : "sensor_"+req.reference;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

RequestResponse FluorescencePlugin::_addDiskSensor(const AddDiskSensorRequest& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp)

    if(!handleCommonSensorParams(req, resp))
        return resp;

    brayns::PropertyMap metaObject;

    metaObject.setProperty({"sensor_shape_type", std::string("disk")});

    addCommonSensorParams(req, metaObject);

    metaObject.setProperty({"sensor_shape_height", static_cast<double>(req.height)});
    metaObject.setProperty({"sensor_shape_radius", static_cast<double>(req.radius)});
    metaObject.setProperty({"sensor_shape_innerradius", static_cast<double>(req.innerRadius)});
    metaObject.setProperty({"sensor_shape_phimax", static_cast<double>(req.phi)});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.reference.empty()? "sensor" : "sensor_"+req.reference;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

RequestResponse FluorescencePlugin::_addFluorescentVolume(const AddFluorescentVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});

    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse
FluorescencePlugin::_addFluorescentAnnotatedVolume(const AddFluorescentAnnotatedVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fexs.size() % 3 != req.ntags)
    {
        resp.error = 4;
        resp.message = "The number of fex elements must be multiple of 3 and the same amount "
                       "as ntags";
        return resp;
    }
    if(req.fems.size() % 3 != req.ntags)
    {
        resp.error = 5;
        resp.message = "The number of fem elements must be multiple of 3 and the same amount "
                       "as ntags";
        return resp;
    }
    if(static_cast<int>(req.epsilons.size()) != req.ntags)
    {
        resp.error = 6;
        resp.message = "The number of epsilon elements must be the same as ntags";
        return resp;
    }
    if(static_cast<int>(req.cs.size()) != req.ntags)
    {
        resp.error = 7;
        resp.message = "The number of c elements must be the same as ntags";
        return resp;
    }
    if(static_cast<int>(req.yields.size()) != req.ntags)
    {
        resp.error = 8;
        resp.message = "The number of yield elements must be the same as ntags";
        return resp;
    }
    if(static_cast<int>(req.gfs.size()) != req.ntags)
    {
        resp.error = 9;
        resp.message = "The number of gf elements must be the same as ntags";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_annotated")});
    metaObject.setProperty({"g", static_cast<double>(req.g)});
    metaObject.setProperty({"ntags", static_cast<int>(req.ntags)});
    metaObject.setProperty({"fexs", req.fexs});
    metaObject.setProperty({"fems", req.fems});
    metaObject.setProperty({"epsilons", req.epsilons});
    metaObject.setProperty({"cs", req.cs});
    metaObject.setProperty({"yields", req.yields});
    metaObject.setProperty({"gfs", req.gfs});
    metaObject.setProperty({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse
FluorescencePlugin::_addFluorescentBinaryVolume(const AddFluorescentBinaryVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_binary")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});

    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});
    metaObject.setProperty({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse
FluorescencePlugin::_addFluorescentGridVolumeFromFile(const AddFluorescentGridFromFileVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_grid")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});

    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});
    metaObject.setProperty({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse FluorescencePlugin::_addFluorescentGridVolume(const AddFluorescentGridVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }
    if((req.nx * req.ny * req.nz) != static_cast<int>(req.density.size()))
    {
        resp.error = 6;
        resp.message = "The grid dimensions (nx * ny * nz) must be equal to the number of "
                       "density elements";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_grid")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});

    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});
    metaObject.setProperty({"density", req.density});
    metaObject.setProperty({"nx", req.nx});
    metaObject.setProperty({"ny", req.ny});
    metaObject.setProperty({"nz", req.nz});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse
FluorescencePlugin::_addFluorescentScatteringVolume(const AddFluorescentScatteringVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }
    if(req.absorption.size() < 3)
    {
        resp.error = 6;
        resp.message = "Absorption parameter requires 3 floats";
        return resp;
    }
    if(req.scattering.size() < 3)
    {
        resp.error = 7;
        resp.message = "Scattering parameter requires 3 floats";
        return resp;
    }
    if(req.Le.size() < 3)
    {
        resp.error = 8;
        resp.message = "Le parameter requires 3 floats";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_scattering")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});
    const std::array<double, 3> a = {req.absorption[0], req.absorption[1], req.absorption[2]};
    metaObject.setProperty({"a", a});
    const std::array<double, 3> s = {req.scattering[0], req.scattering[1], req.scattering[2]};
    metaObject.setProperty({"s", s});
    const std::array<double, 3> Le = {req.Le[0], req.Le[1], req.Le[2]};
    metaObject.setProperty({"Le", Le});

    metaObject.setProperty({"mweight", static_cast<double>(req.mweight)});
    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"sscale", static_cast<double>(req.sscale)});
    metaObject.setProperty({"fscale", static_cast<double>(req.fscale)});
    metaObject.setProperty({"g", static_cast<double>(req.g)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});
    metaObject.setProperty({"density", static_cast<double>(req.density)});


    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse
FluorescencePlugin::_addFluorescentScatteringGridVolume(
        const AddFluorescentScatteringGridVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }
    if(req.absorption.size() < 3)
    {
        resp.error = 6;
        resp.message = "Absorption parameter requires 3 floats";
        return resp;
    }
    if(req.scattering.size() < 3)
    {
        resp.error = 7;
        resp.message = "Scattering parameter requires 3 floats";
        return resp;
    }
    if(req.Le.size() < 3)
    {
        resp.error = 8;
        resp.message = "Le parameter requires 3 floats";
        return resp;
    }
    if((req.nx * req.ny * req.nz) != static_cast<int>(req.density.size()))
    {
        resp.error = 9;
        resp.message = "The grid dimensions (nx * ny * nz) must be equal to the number of "
                       "density elements";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_scattering_grid")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});
    const std::array<double, 3> a = {req.absorption[0], req.absorption[1], req.absorption[2]};
    metaObject.setProperty({"a", a});
    const std::array<double, 3> s = {req.scattering[0], req.scattering[1], req.scattering[2]};
    metaObject.setProperty({"s", s});
    const std::array<double, 3> Le = {req.Le[0], req.Le[1], req.Le[2]};
    metaObject.setProperty({"Le", Le});

    metaObject.setProperty({"mweight", static_cast<double>(req.mweight)});
    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"sscale", static_cast<double>(req.sscale)});
    metaObject.setProperty({"fscale", static_cast<double>(req.fscale)});
    metaObject.setProperty({"g", static_cast<double>(req.g)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});
    metaObject.setProperty({"density", req.density});
    metaObject.setProperty({"nx", req.nx});
    metaObject.setProperty({"ny", req.ny});
    metaObject.setProperty({"nz", req.nz});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

RequestResponse
FluorescencePlugin::_addFluorescentScatteringGridVolumeFromFile(
        const AddFluorescentScatteringGridFromFileVolume& req)
{
    RequestResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp);

    if(!handleCommonVolumeParams(req, resp))
        return resp;

    if(req.fex.size() < 3)
    {
        resp.error = 4;
        resp.message = "Fex parameter requires 3 floats";
        return resp;
    }
    if(req.fem.size() < 3)
    {
        resp.error = 5;
        resp.message = "Fem parameter requires 3 floats";
        return resp;
    }
    if(req.absorption.size() < 3)
    {
        resp.error = 6;
        resp.message = "Absorption parameter requires 3 floats";
        return resp;
    }
    if(req.scattering.size() < 3)
    {
        resp.error = 7;
        resp.message = "Scattering parameter requires 3 floats";
        return resp;
    }
    if(req.Le.size() < 3)
    {
        resp.error = 8;
        resp.message = "Le parameter requires 3 floats";
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.setProperty({"volume_type", std::string("fluorescence_scattering_grid")});

    const std::array<double, 3> fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.setProperty({"fex", fex});
    const std::array<double, 3> fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.setProperty({"fem", fem});
    const std::array<double, 3> a = {req.absorption[0], req.absorption[1], req.absorption[2]};
    metaObject.setProperty({"a", a});
    const std::array<double, 3> s = {req.scattering[0], req.scattering[1], req.scattering[2]};
    metaObject.setProperty({"s", s});
    const std::array<double, 3> Le = {req.Le[0], req.Le[1], req.Le[2]};
    metaObject.setProperty({"Le", Le});

    metaObject.setProperty({"mweight", static_cast<double>(req.mweight)});
    metaObject.setProperty({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.setProperty({"c", static_cast<double>(req.c)});
    metaObject.setProperty({"yield", static_cast<double>(req.yield)});
    metaObject.setProperty({"sscale", static_cast<double>(req.sscale)});
    metaObject.setProperty({"fscale", static_cast<double>(req.fscale)});
    metaObject.setProperty({"g", static_cast<double>(req.g)});
    metaObject.setProperty({"gf", static_cast<double>(req.gf)});
    metaObject.setProperty({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing Fluorescence plugin" << std::endl;
    return new FluorescencePlugin();
}
