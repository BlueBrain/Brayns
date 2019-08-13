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

TEST_CASE_FIXTURE(ClientServer, "set_properties")
{
    auto model = getScene().getModel(0);

    brayns::PropertyMap props;
    props.setProperty({"bla", 0});
    model->setProperties(props);

    brayns::PropertyMap propsNew;
    propsNew.setProperty({"bla", 42});
    CHECK((makeRequest<brayns::ModelProperties, bool>(
        SET_PROPERTIES, {model->getModelID(), propsNew})));

    CHECK_EQ(model->getProperties().getProperty<int32_t>("bla"), 42);

    SUBCASE("get_model_properties_schema")
    {
        auto result =
            makeRequestJSONReturn<brayns::ObjectID>(MODEL_PROPERTIES_SCHEMA,
                                                    {model->getModelID()});

        using namespace rapidjson;
        Document json(kObjectType);
        json.Parse(result.c_str());
        CHECK(json["properties"].HasMember("bla"));
    }

    SUBCASE("get_model_properties")
    {
        auto result = makeRequestUpdate<brayns::ObjectID, brayns::PropertyMap>(
            GET_PROPERTIES, {model->getModelID()}, props);
        REQUIRE(result.hasProperty("bla"));
        CHECK_EQ(result.getProperty<int32_t>("bla"), 42);
    }
}

TEST_CASE_FIXTURE(ClientServer, "update_model")
{
    auto model = getScene().getModel(0);

    const uint32_t id = model->getModelID();

    model->setBoundingBox(true); // different from default

    CHECK(model->getVisible());
    CHECK(model->getBoundingBox());

    // create partial model description to only update visible state
    using namespace rapidjson;
    Document json(kObjectType);
    json.AddMember("id", id, json.GetAllocator());
    json.AddMember("visible", false, json.GetAllocator());
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    json.Accept(writer);

    CHECK((makeRequest<bool>(UPDATE_MODEL, buffer.GetString())));

    CHECK(!model->getVisible());
    CHECK(model->getBoundingBox()); // shall remain untouched
}

TEST_CASE_FIXTURE(ClientServer, "instances")
{
    auto model = getScene().getModel(0);

    brayns::Transformation trafo;
    trafo.setTranslation({10, 200, -400});
    model->addInstance({true, false, trafo});
    process();

    SUBCASE("add_instance")
    {
        REQUIRE_EQ(model->getInstances().size(), 2);
        auto& instance = model->getInstances()[1];
        CHECK(instance.getTransformation() == trafo);
        CHECK(instance.getVisible());
        CHECK(!instance.getBoundingBox());

        const auto& instances =
            makeRequest<brayns::GetInstances, brayns::ModelInstances>(
                GET_INSTANCES, {model->getModelID(), {1, 2}});
        REQUIRE_EQ(instances.size(), 1);
        auto& rpcInstance = instances[0];
        CHECK(rpcInstance.getTransformation() == trafo);
        CHECK(rpcInstance.getVisible());
        CHECK(!rpcInstance.getBoundingBox());
    }

    SUBCASE("update_instance")
    {
        REQUIRE_EQ(model->getInstances().size(), 2);
        auto instance = model->getInstances()[1];

        instance.setBoundingBox(true);
        instance.setVisible(false);
        auto scaleTrafo = instance.getTransformation();
        scaleTrafo.setScale({1, 2, 3});
        instance.setTransformation(scaleTrafo);

        CHECK(makeRequest<brayns::ModelInstance, bool>(UPDATE_INSTANCE,
                                                       instance));

        process();

        const auto& updatedInstance = model->getInstances()[1];
        CHECK(updatedInstance.getTransformation() == scaleTrafo);
        CHECK(!updatedInstance.getVisible());
        CHECK(updatedInstance.getBoundingBox());
    }

    SUBCASE("remove_instance")
    {
        REQUIRE_EQ(model->getInstances().size(), 2);

        model->removeInstance(model->getInstances()[1].getInstanceID());
        process();

        REQUIRE_EQ(model->getInstances().size(), 1);
    }
}

TEST_CASE_FIXTURE(ClientServer, "remove_model")
{
    const auto desc = getScene().getModel(0);

    CHECK(makeRequest<size_ts, bool>(REMOVE_MODEL, {desc->getModelID()}));

    CHECK_EQ(getScene().getNumModels(), 0);
}
