/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#define BOOST_TEST_MODULE braynsModel

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <jsonPropertyMap.h>
#include <jsonSerialization.h>

#include "ClientServer.h"

const std::string GET_INSTANCES("get-instances");
const std::string REMOVE_MODEL("remove-model");
const std::string UPDATE_INSTANCE("update-instance");
const std::string UPDATE_MODEL("update-model");
const std::string SET_PROPERTIES("set-model-properties");
const std::string GET_PROPERTIES("get-model-properties");
const std::string MODEL_PROPERTIES_SCHEMA("model-properties-schema");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(set_properties)
{
    auto model = getScene().getModel(0);

    brayns::PropertyMap props;
    props.setProperty({"bla", "bla property", 0});
    model->setProperties(props);

    brayns::PropertyMap propsNew;
    propsNew.setProperty({"bla", "bla property", 42});
    BOOST_CHECK((makeRequest<brayns::ModelProperties, bool>(
        SET_PROPERTIES, {model->getModelID(), propsNew})));

    BOOST_CHECK_EQUAL(model->getProperties().getProperty<int32_t>("bla"), 42);

    auto result = makeRequestUpdate<brayns::ObjectID, brayns::PropertyMap>(
        GET_PROPERTIES, {model->getModelID()}, props);
    BOOST_REQUIRE(result.hasProperty("bla"));
    BOOST_CHECK_EQUAL(result.getProperty<int32_t>("bla"), 42);
}

BOOST_AUTO_TEST_CASE(model_properties_schema)
{
    auto model = getScene().getModel(0);

    auto result =
        makeRequestJSONReturn<brayns::ObjectID>(MODEL_PROPERTIES_SCHEMA,
                                                {model->getModelID()});

    using namespace rapidjson;
    Document json(kObjectType);
    json.Parse(result.c_str());
    BOOST_CHECK(json["properties"].HasMember("bla"));
}

BOOST_AUTO_TEST_CASE(update_model)
{
    auto model = getScene().getModel(0);

    const uint32_t id = model->getModelID();

    model->setBoundingBox(true); // different from default

    BOOST_CHECK(model->getVisible());
    BOOST_CHECK(model->getBoundingBox());

    // create partial model description to only update visible state
    using namespace rapidjson;
    Document json(kObjectType);
    json.AddMember("id", id, json.GetAllocator());
    json.AddMember("visible", false, json.GetAllocator());
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    json.Accept(writer);

    BOOST_CHECK((makeRequest<bool>(UPDATE_MODEL, buffer.GetString())));

    BOOST_CHECK(!model->getVisible());
    BOOST_CHECK(model->getBoundingBox()); // shall remain untouched
}

BOOST_AUTO_TEST_CASE(add_instance)
{
    auto model = getScene().getModel(0);

    brayns::Transformation trafo;
    trafo.setTranslation({10, 200, -400});
    model->addInstance({true, false, trafo});
    process();

    BOOST_REQUIRE_EQUAL(model->getInstances().size(), 2);
    auto& instance = model->getInstances()[1];
    BOOST_CHECK(instance.getTransformation() == trafo);
    BOOST_CHECK(instance.getVisible());
    BOOST_CHECK(!instance.getBoundingBox());

    const auto& instances =
        makeRequest<brayns::GetInstances, brayns::ModelInstances>(
            GET_INSTANCES, {model->getModelID(), {1, 2}});
    BOOST_REQUIRE_EQUAL(instances.size(), 1);
    auto& rpcInstance = instances[0];
    BOOST_CHECK(rpcInstance.getTransformation() == trafo);
    BOOST_CHECK(rpcInstance.getVisible());
    BOOST_CHECK(!rpcInstance.getBoundingBox());
}

BOOST_AUTO_TEST_CASE(update_instance)
{
    auto model = getScene().getModel(0);
    BOOST_REQUIRE_EQUAL(model->getInstances().size(), 2);
    auto instance = model->getInstances()[1];

    instance.setBoundingBox(true);
    instance.setVisible(false);
    auto trafo = instance.getTransformation();
    trafo.setScale({1, 2, 3});
    instance.setTransformation(trafo);

    BOOST_CHECK(
        (makeRequest<brayns::ModelInstance, bool>(UPDATE_INSTANCE, instance)));

    process();

    const auto& updatedInstance = model->getInstances()[1];
    BOOST_CHECK(updatedInstance.getTransformation() == trafo);
    BOOST_CHECK(!updatedInstance.getVisible());
    BOOST_CHECK(updatedInstance.getBoundingBox());
}

BOOST_AUTO_TEST_CASE(remove_instance)
{
    auto model = getScene().getModel(0);
    BOOST_REQUIRE_EQUAL(model->getInstances().size(), 2);

    model->removeInstance(model->getInstances()[1].getInstanceID());
    process();

    BOOST_REQUIRE_EQUAL(model->getInstances().size(), 1);
}

BOOST_AUTO_TEST_CASE(remove_model)
{
    const auto desc = getScene().getModel(0);

    BOOST_CHECK(
        (makeRequest<size_ts, bool>(REMOVE_MODEL, {desc->getModelID()})));

    BOOST_CHECK_EQUAL(getScene().getNumModels(), 0);
}
