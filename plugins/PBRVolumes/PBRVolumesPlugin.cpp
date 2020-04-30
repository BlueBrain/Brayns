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

#include "PBRVolumesPlugin.h"
#include "Log.h"

#include <brayns/common/ActionInterface.h>
#include <brayns/common/PropertyMap.h>

#include <brayns/common/geometry/TriangleMesh.h>

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
inline bool handleCommonParams(const Request& req, AddVolumeResponse& resp)
{
    if(req.absorption.size() < 3)
    {
        resp.error = 2;
        resp.message = "Absroption coefficent requires 3 float numbers";
        return false;
    }

    if(req.scattering.size() < 3)
    {
        resp.error = 3;
        resp.message = "Scattering coefficent requires 3 float numbers";
        return false;
    }

    if(req.Le.size() < 3)
    {
        resp.error = 4;
        resp.message = "Le parameter requires 3 float numbers";
        return false;
    }

    return true;
}

template<typename Request>
inline bool handleBoxParams(const Request& req, AddVolumeResponse& resp)
{
    if(req.p0.size() < 3)
    {
        resp.error = 5;
        resp.message = "Minimum bound (p0) parameter requires 3 floats";
        return false;
    }

    if(req.p1.size() < 3)
    {
        resp.error = 6;
        resp.message = "Maximum bound (p1) parameter requires 3 floats";
        return false;
    }

    return true;
}

template<typename Request>
inline void addCommonParams(const Request& req, brayns::PropertyMap& metaObject)
{
    const std::array<double, 3> absoption = {req.absorption[0],
                                             req.absorption[1],
                                             req.absorption[2]};
    metaObject.setProperty({"sig_a", absoption});

    const std::array<double, 3> scattering = {req.scattering[0],
                                              req.scattering[1],
                                              req.scattering[2]};
    metaObject.setProperty({"sig_s", scattering});

    metaObject.setProperty({"scale", static_cast<double>(req.scale)});
    metaObject.setProperty({"g", static_cast<double>(req.g)});

    const std::array<double, 3> Le = {req.Le[0],
                                      req.Le[1],
                                      req.Le[2]};
    metaObject.setProperty({"Le", Le});
}

template<typename Request>
inline void addBoxParams(const Request& req, brayns::PropertyMap& metaObject)
{
    const std::array<double, 3> p0 = {req.p0[0], req.p0[1], req.p0[2]};
    metaObject.setProperty({"p0", p0});
    const std::array<double, 3> p1 = {req.p1[0], req.p1[1], req.p1[2]};
    metaObject.setProperty({"p1", p1});
}

PBRVolumesPlugin::PBRVolumesPlugin()
 : brayns::ExtensionPlugin()
{
}

PBRVolumesPlugin::~PBRVolumesPlugin()
{
}

void PBRVolumesPlugin::init()
{
    auto actionInterface = _api->getActionInterface();
    if(actionInterface)
    {
        PLUGIN_INFO << "Registering 'add-homogeneus-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddHomogeneusVolume, AddVolumeResponse>(
            "add-homogeneus-volume", [&](const AddHomogeneusVolume& request){
                return _addHomogeneusVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-homogeneus-volume-model' endpoint" << std::endl;
        actionInterface->registerRequest<AddHomogeneusVolumeToModel, AddVolumeResponse>(
            "add-homogeneus-volume-model", [&](const AddHomogeneusVolumeToModel& request){
                return _addHomogeneusVolumeToModel(request);
        });

        PLUGIN_INFO << "Registering 'add-heterogeneus-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddHeterogeneusVolume, AddVolumeResponse>(
            "add-heterogeneus-volume", [&](const AddHeterogeneusVolume& request){
                return _addHeterogeneusVolume(request);
        });

        PLUGIN_INFO << "Registering 'add-grid-volume' endpoint" << std::endl;
        actionInterface->registerRequest<AddGridVolume, AddVolumeResponse>(
            "add-grid-volume", [&](const AddGridVolume& request){
                return _addGridVolume(request);
        });
    }
}

AddVolumeResponse PBRVolumesPlugin::_addHomogeneusVolume(const AddHomogeneusVolume& req)
{
    AddVolumeResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp)

    if(!handleCommonParams(req, resp))
        return resp;

    if(!handleBoxParams(req, resp))
        return resp;

    brayns::PropertyMap metaObject;

    metaObject.setProperty({"volume_type", std::string("homogeneus")});
    metaObject.setProperty({"density", static_cast<double>(req.density)});
    addCommonParams(req, metaObject);
    addBoxParams(req, metaObject);

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

AddVolumeResponse
PBRVolumesPlugin::_addHomogeneusVolumeToModel(const AddHomogeneusVolumeToModel& req)
{
    AddVolumeResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp)

    if(!handleCommonParams(req, resp))
        return resp;

    const size_t modelId = static_cast<size_t>(req.modelId);
    auto modelPtr = _api->getScene().getModel(modelId);
    if(!modelPtr)
    {
        resp.error = 5;
        resp.message = "The given model ID does not exist";
        return resp;
    }

    brayns::PropertyMap metaObject;
    metaObject.setProperty({"volume_type", std::string("homogeneus")});
    metaObject.setProperty({"density", static_cast<double>(req.density)});
    addCommonParams(req, metaObject);

    modelPtr->getModel().addMetaObject(brayns::NO_MATERIAL, metaObject);
    modelPtr->markModified();
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

AddVolumeResponse PBRVolumesPlugin::_addHeterogeneusVolume(const AddHeterogeneusVolume& req)
{
    AddVolumeResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp)

    if(!handleCommonParams(req, resp))
        return resp;

    if(!handleBoxParams(req, resp))
        return resp;

    brayns::PropertyMap metaObject;

    metaObject.setProperty({"volume_type", std::string("heterogeneus")});
    metaObject.setProperty({"min_density", static_cast<double>(req.minDensity)});
    metaObject.setProperty({"max_density", static_cast<double>(req.maxDensity)});
    addCommonParams(req, metaObject);
    addBoxParams(req, metaObject);

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

AddVolumeResponse PBRVolumesPlugin::_addGridVolume(const AddGridVolume& req)
{
    AddVolumeResponse resp;
    resp.error = 0;
    resp.message = "";

    HANDLE_NOT_PARSED(req, resp)

    if(!handleCommonParams(req, resp))
        return resp;

    if(!handleBoxParams(req, resp))
        return resp;

    if(req.nx < 1 || req.ny < 1 || req.nz < 1)
    {
        resp.error = 7;
        resp.message = "The grid dimensions must be positive non zero values";
        return resp;
    }

    if(static_cast<size_t>(req.nx * req.ny * req.nz) != req.density.size())
    {
        resp.error = 8;
        resp.message = "The grid dimensions (nx x ny x nz) must match the number of denisty"
                       " values";
        return resp;
    }

    brayns::PropertyMap metaObject;

    metaObject.setProperty({"volume_type", std::string("grid")});
    metaObject.setProperty({"grid_type", req.gridType});
    metaObject.setProperty({"density", req.density});
    metaObject.setProperty({"nx", req.nx});
    metaObject.setProperty({"ny", req.ny});
    metaObject.setProperty({"nz", req.nz});
    addCommonParams(req, metaObject);
    addBoxParams(req, metaObject);

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty()? "Volume" : req.name;
    _api->getScene().addModel(std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr),
                                                                        name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing Physically Based Volumes plugin" << std::endl;
    return new PBRVolumesPlugin();
}
