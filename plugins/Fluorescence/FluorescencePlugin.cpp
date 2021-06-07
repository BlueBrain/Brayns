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
#include <brayns/common/propertymap/PropertyMap.h>

#include <brayns/engine/Engine.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/pluginapi/PluginAPI.h>

template <typename Request>
inline bool handleCommonSensorParams(const Request& req, brayns::Message& resp)
{
    if (req.rotation.size() < 4)
    {
        resp.setError(2,
                      "Rotation param requires 4 float numbers (angle + axis)");
        return false;
    }

    if (req.translation.size() < 3)
    {
        resp.setError(3, "Translation param requires 3 float numbers");
        return false;
    }

    if (req.scale.size() < 3)
    {
        resp.setError(4, "Scale param requires 3 float numbers");
        return false;
    }

    return true;
}

template <typename Request>
inline bool handleCommonVolumeParams(const Request& req, brayns::Message& resp)
{
    if (req.p0.size() < 3)
    {
        resp.setError(2, "Volume minimun bound (p0) requires 3 float values");
        return false;
    }

    if (req.p1.size() < 3)
    {
        resp.setError(3, "Volume maximun bound (p1) requires 3 float values");
        return false;
    }

    return true;
}

template <typename Request>
inline void addCommonSensorParams(const Request& req,
                                  brayns::PropertyMap& metaObject)
{
    const Vector4d rotation = {req.rotation[0], req.rotation[1],
                               req.rotation[2], req.rotation[3]};
    metaObject.add({"sensor_rotation", rotation});

    const Vector3d translation = {req.translation[0], req.translation[1],
                                  req.translation[2]};
    metaObject.add({"sensor_translation", translation});

    const Vector3d scale = {req.scale[0], req.scale[1], req.scale[2]};
    metaObject.add({"sensor_scale", scale});

    metaObject.add({"reference", req.reference});
    metaObject.add({"xpixels", req.xpixels});
    metaObject.add({"ypixels", req.ypixels});
    metaObject.add({"fov", static_cast<double>(req.fov)});
}

template <typename Request>
inline void addCommonVolumeParams(const Request& req,
                                  brayns::PropertyMap& metaObject)
{
    const Vector3d p0 = {req.p0[0], req.p0[1], req.p0[2]};
    metaObject.add({"p0", p0});
    const Vector3d p1 = {req.p1[0], req.p1[1], req.p1[2]};
    metaObject.add({"p1", p1});
}

FluorescencePlugin::FluorescencePlugin()
    : ExtensionPlugin()
{
}

FluorescencePlugin::~FluorescencePlugin() {}

void FluorescencePlugin::init()
{
    auto actionInterface = _api->getActionInterface();
    if (actionInterface)
    {
        actionInterface
            ->registerRequest<AddRectangleSensorRequest, brayns::Message>(
                {"add-rectangle-sensor",
                 "Adds a rectangle shaped sensor to the scene",
                 "AddRectangleSensorRequest",
                 "Parameters to define the rectangle sensor"},
                [&](const AddRectangleSensorRequest& request)
                { return _addRectangleSensor(request); });

        actionInterface->registerRequest<AddDiskSensorRequest, brayns::Message>(
            {"add-disk-sensor", "Adds a disk shaped sensor to the scene",
             "AddDiskSensorRequest", "Parameters to define the disk sensor"},
            [&](const AddDiskSensorRequest& request)
            { return _addDiskSensor(request); });

        actionInterface->registerRequest<AddFluorescentVolume, brayns::Message>(
            {"add-fluorescent-volume",
             "Adds a constant density fluorescent volume to the scene",
             "AddFluorescentVolume",
             "Parameters to define the fluorescent volume"},
            [&](const AddFluorescentVolume& request)
            { return _addFluorescentVolume(request); });

        actionInterface
            ->registerRequest<AddFluorescentAnnotatedVolume, brayns::Message>(
                {"add-fluorescent-ann-volume",
                 "Adds an annotated fluorescent volume to the scene",
                 "AddFluorescentAnnotatedVolume",
                 "Parameters to define an annotated fluorescent volume"},
                [&](const AddFluorescentAnnotatedVolume& request)
                { return _addFluorescentAnnotatedVolume(request); });

        actionInterface->registerRequest<AddFluorescentBinaryVolume,
                                         brayns::Message>(
            {"add-fluorescent-bin-volume",
             "Adds a constant density binary fluorescent volume to the scene",
             "AddFluorescentBinaryVolume",
             "Parameters to define a binary fluorescent volume"},
            [&](const AddFluorescentBinaryVolume& request)
            { return _addFluorescentBinaryVolume(request); });

        actionInterface->registerRequest<AddFluorescentGridFromFileVolume,
                                         brayns::Message>(
            {"add-fluorescent-grid-file-volume",
             "Adds a fluorescent grid volume, whose density values are loaded "
             "from a file on disk",
             "AddFluorescentGridFromFileVolume",
             "Parameters to define a fluorescent volume and the path to the "
             "density file"},
            [&](const AddFluorescentGridFromFileVolume& request)
            { return _addFluorescentGridVolumeFromFile(request); });

        actionInterface
            ->registerRequest<AddFluorescentGridVolume, brayns::Message>(
                {"add-fluorescent-grid-volume",
                 "Adds a fluorescent grid volume, whose density values are "
                 "specified in the request",
                 "AddFluorescentGridVolume",
                 "Parameters to define a fluorescent grid volume, and density "
                 "values and dimension of"
                 " the grid"},
                [&](const AddFluorescentGridVolume& request)
                { return _addFluorescentGridVolume(request); });

        actionInterface
            ->registerRequest<AddFluorescentScatteringVolume, brayns::Message>(
                {"add-fluorescent-scatter-volume",
                 "Adds a constant density fluorescent scattering volume to the "
                 "scene",
                 "AddFluorescentScatteringVolume",
                 "Parameters to define the fluorescent scattering volume"},
                [&](const AddFluorescentScatteringVolume& request)
                { return _addFluorescentScatteringVolume(request); });

        actionInterface->registerRequest<AddFluorescentScatteringGridVolume,
                                         brayns::Message>(
            {"add-fluorescent-scattergrid-volume",
             "Adds a fluorescent grid scattering volume to the scene",
             "AddFluorescentScatteringGridVolume",
             "Parameters to define a fluorescent scattering grid volume, and "
             "the density values"
             " and dimensions of the grid"},
            [&](const AddFluorescentScatteringGridVolume& request)
            { return _addFluorescentScatteringGridVolume(request); });

        actionInterface->registerRequest<
            AddFluorescentScatteringGridFromFileVolume, brayns::Message>(
            {"add-fluorescent-scattergridfile-volume",
             "Adds a fluorescent scattering volume to the scene, whose values "
             "are loaded from a "
             "file on disk",
             "AddFluorescentScatteringGridFromFileVolume",
             "Parameters to define the fluorescent scattering grid volume, and "
             "the path to the "
             "density file"},
            [&](const AddFluorescentScatteringGridFromFileVolume& request)
            { return _addFluorescentScatteringGridVolumeFromFile(request); });
    }
}

brayns::Message FluorescencePlugin::_addRectangleSensor(
    const AddRectangleSensorRequest& req)
{
    brayns::Message resp;

    if (!handleCommonSensorParams(req, resp))
        return resp;

    brayns::PropertyMap metaObject;

    metaObject.add({"sensor_shape_type", std::string("rectangle")});

    addCommonSensorParams(req, metaObject);

    metaObject.add({"sensor_shape_height", req.height});
    metaObject.add({"sensor_shape_x", req.x});
    metaObject.add({"sensor_shape_y", req.y});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name =
        req.reference.empty() ? "sensor" : "sensor_" + req.reference;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addDiskSensor(
    const AddDiskSensorRequest& req)
{
    brayns::Message resp;

    if (!handleCommonSensorParams(req, resp))
        return resp;

    brayns::PropertyMap metaObject;

    metaObject.add({"sensor_shape_type", std::string("disk")});

    addCommonSensorParams(req, metaObject);

    metaObject.add({"sensor_shape_height", req.height});
    metaObject.add({"sensor_shape_radius", req.radius});
    metaObject.add({"sensor_shape_innerradius", req.innerRadius});
    metaObject.add({"sensor_shape_phimax", req.phi});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name =
        req.reference.empty() ? "sensor" : "sensor_" + req.reference;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentVolume(
    const AddFluorescentVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add({"volume_type", std::string("fluorescence")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});

    metaObject.add({"epsilon", req.epsilon});
    metaObject.add({"c", req.c});
    metaObject.add({"yield", req.yieldV});
    metaObject.add({"gf", req.gf});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentAnnotatedVolume(
    const AddFluorescentAnnotatedVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fexs.size() % 3 != req.ntags)
    {
        resp.setError(4,
                      "The number of fex elements must be multiple of 3 and "
                      "the same amount "
                      "as ntags");
        return resp;
    }
    if (req.fems.size() % 3 != req.ntags)
    {
        resp.setError(5,
                      "The number of fem elements must be multiple of 3 and "
                      "the same amount "
                      "as ntags");
        return resp;
    }
    if (static_cast<int32_t>(req.epsilons.size()) != req.ntags)
    {
        resp.setError(
            6, "The number of epsilon elements must be the same as ntags");
        return resp;
    }
    if (static_cast<int32_t>(req.cs.size()) != req.ntags)
    {
        resp.setError(7, "The number of c elements must be the same as ntags");
        return resp;
    }
    if (static_cast<int32_t>(req.yields.size()) != req.ntags)
    {
        resp.setError(8,
                      "The number of yield elements must be the same as ntags");
        return resp;
    }
    if (static_cast<int>(req.gfs.size()) != req.ntags)
    {
        resp.setError(9, "The number of gf elements must be the same as ntags");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add({"volume_type", std::string("fluorescence_annotated")});
    metaObject.add({"g", req.g});
    metaObject.add({"ntags", req.ntags});
    metaObject.add({"fexs", req.fexs});
    metaObject.add({"fems", req.fems});
    metaObject.add({"epsilons", req.epsilons});
    metaObject.add({"cs", req.cs});
    metaObject.add({"yields", req.yields});
    metaObject.add({"gfs", req.gfs});
    metaObject.add({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentBinaryVolume(
    const AddFluorescentBinaryVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add({"volume_type", std::string("fluorescence_binary")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});

    metaObject.add({"epsilon", req.epsilon});
    metaObject.add({"c", req.c});
    metaObject.add({"yield", req.yieldV});
    metaObject.add({"gf", req.gf});
    metaObject.add({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentGridVolumeFromFile(
    const AddFluorescentGridFromFileVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add({"volume_type", std::string("fluorescence_grid")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});

    metaObject.add({"epsilon", req.epsilon});
    metaObject.add({"c", req.c});
    metaObject.add({"yield", req.yieldV});
    metaObject.add({"gf", req.gf});
    metaObject.add({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentGridVolume(
    const AddFluorescentGridVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }
    if ((req.nx * req.ny * req.nz) != static_cast<int32_t>(req.density.size()))
    {
        resp.setError(
            6,
            "The grid dimensions (nx * ny * nz) must be equal to the number of "
            "density elements");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add({"volume_type", std::string("fluorescence_grid")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});

    metaObject.add({"epsilon", req.epsilon});
    metaObject.add({"c", req.c});
    metaObject.add({"yield", req.yieldV});
    metaObject.add({"gf", req.gf});
    metaObject.add({"density", req.density});
    metaObject.add({"nx", req.nx});
    metaObject.add({"ny", req.ny});
    metaObject.add({"nz", req.nz});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentScatteringVolume(
    const AddFluorescentScatteringVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }
    if (req.absorption.size() < 3)
    {
        resp.setError(6, "Absorption parameter requires 3 floats");
        return resp;
    }
    if (req.scattering.size() < 3)
    {
        resp.setError(7, "Scattering parameter requires 3 floats");
        return resp;
    }
    if (req.Le.size() < 3)
    {
        resp.setError(8, "Le parameter requires 3 floats");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add({"volume_type", std::string("fluorescence_scattering")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});
    const Vector3d a = {req.absorption[0], req.absorption[1],
                        req.absorption[2]};
    metaObject.add({"a", a});
    const Vector3d s = {req.scattering[0], req.scattering[1],
                        req.scattering[2]};
    metaObject.add({"s", s});
    const Vector3d Le = {req.Le[0], req.Le[1], req.Le[2]};
    metaObject.add({"Le", Le});

    metaObject.add({"mweight", req.mweight});
    metaObject.add({"epsilon", req.epsilon});
    metaObject.add({"c", req.c});
    metaObject.add({"yield", req.yieldV});
    metaObject.add({"sscale", req.sscale});
    metaObject.add({"fscale", req.fscale});
    metaObject.add({"g", req.g});
    metaObject.add({"gf", req.gf});
    metaObject.add({"density", req.density});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentScatteringGridVolume(
    const AddFluorescentScatteringGridVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }
    if (req.absorption.size() < 3)
    {
        resp.setError(6, "Absorption parameter requires 3 floats");
        return resp;
    }
    if (req.scattering.size() < 3)
    {
        resp.setError(7, "Scattering parameter requires 3 floats");
        return resp;
    }
    if (req.Le.size() < 3)
    {
        resp.setError(8, "Le parameter requires 3 floats");
        return resp;
    }
    if ((req.nx * req.ny * req.nz) != static_cast<int32_t>(req.density.size()))
    {
        resp.setError(
            9,
            "The grid dimensions (nx * ny * nz) must be equal to the number of "
            "density elements");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add(
        {"volume_type", std::string("fluorescence_scattering_grid")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});
    const Vector3d a = {req.absorption[0], req.absorption[1],
                        req.absorption[2]};
    metaObject.add({"a", a});
    const Vector3d s = {req.scattering[0], req.scattering[1],
                        req.scattering[2]};
    metaObject.add({"s", s});
    const Vector3d Le = {req.Le[0], req.Le[1], req.Le[2]};
    metaObject.add({"Le", Le});

    metaObject.add({"mweight", static_cast<double>(req.mweight)});
    metaObject.add({"epsilon", static_cast<double>(req.epsilon)});
    metaObject.add({"c", static_cast<double>(req.c)});
    metaObject.add({"yield", static_cast<double>(req.yieldV)});
    metaObject.add({"sscale", static_cast<double>(req.sscale)});
    metaObject.add({"fscale", static_cast<double>(req.fscale)});
    metaObject.add({"g", static_cast<double>(req.g)});
    metaObject.add({"gf", static_cast<double>(req.gf)});
    metaObject.add({"density", req.density});
    metaObject.add({"nx", req.nx});
    metaObject.add({"ny", req.ny});
    metaObject.add({"nz", req.nz});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

brayns::Message FluorescencePlugin::_addFluorescentScatteringGridVolumeFromFile(
    const AddFluorescentScatteringGridFromFileVolume& req)
{
    brayns::Message resp;

    if (!handleCommonVolumeParams(req, resp))
        return resp;

    if (req.fex.size() < 3)
    {
        resp.setError(4, "Fex parameter requires 3 floats");
        return resp;
    }
    if (req.fem.size() < 3)
    {
        resp.setError(5, "Fem parameter requires 3 floats");
        return resp;
    }
    if (req.absorption.size() < 3)
    {
        resp.setError(6, "Absorption parameter requires 3 floats");
        return resp;
    }
    if (req.scattering.size() < 3)
    {
        resp.setError(7, "Scattering parameter requires 3 floats");
        return resp;
    }
    if (req.Le.size() < 3)
    {
        resp.setError(8, "Le parameter requires 3 floats");
        return resp;
    }

    brayns::PropertyMap metaObject;
    addCommonVolumeParams(req, metaObject);
    metaObject.add(
        {"volume_type", std::string("fluorescence_scattering_grid")});

    const Vector3d fex = {req.fex[0], req.fex[1], req.fex[2]};
    metaObject.add({"fex", fex});
    const Vector3d fem = {req.fem[0], req.fem[1], req.fem[2]};
    metaObject.add({"fem", fem});
    const Vector3d a = {req.absorption[0], req.absorption[1],
                        req.absorption[2]};
    metaObject.add({"a", a});
    const Vector3d s = {req.scattering[0], req.scattering[1],
                        req.scattering[2]};
    metaObject.add({"s", s});
    const Vector3d Le = {req.Le[0], req.Le[1], req.Le[2]};
    metaObject.add({"Le", Le});

    metaObject.add({"mweight", req.mweight});
    metaObject.add({"epsilon", req.epsilon});
    metaObject.add({"c", req.c});
    metaObject.add({"yield", req.yieldV});
    metaObject.add({"sscale", req.sscale});
    metaObject.add({"fscale", req.fscale});
    metaObject.add({"g", req.g});
    metaObject.add({"gf", req.gf});
    metaObject.add({"prefix", req.prefix});

    auto modelPtr = _api->getScene().createModel();
    modelPtr->addMetaObject(brayns::NO_MATERIAL, metaObject);
    const std::string name = req.name.empty() ? "Volume" : req.name;
    _api->getScene().addModel(
        std::make_shared<brayns::ModelDescriptor>(std::move(modelPtr), name));
    _api->getScene().markModified();
    _api->getEngine().triggerRender();

    return resp;
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing Fluorescence plugin" << std::endl;
    return new FluorescencePlugin();
}
