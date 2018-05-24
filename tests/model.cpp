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
#include <jsonSerialization.h>

#include "ClientServer.h"

const std::string GET_INSTANCES("get-instances");
const std::string REMOVE_MODEL("remove-model");
const std::string UPDATE_INSTANCE("update-instance");
const std::string UPDATE_MODEL("update-model");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(update_model)
{
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    const uint32_t id = models[0]->getModelID();

    models[0]->setBoundingBox(true); // different from default

    BOOST_CHECK(models[0]->getVisible());
    BOOST_CHECK(models[0]->getBoundingBox());

    // create partial model description to only update visible state
    using namespace rapidjson;
    Document json(kObjectType);
    json.AddMember("id", id, json.GetAllocator());
    json.AddMember("visible", false, json.GetAllocator());
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    json.Accept(writer);

    BOOST_CHECK((makeRequest<bool>(UPDATE_MODEL, buffer.GetString())));

    BOOST_CHECK(!models[0]->getVisible());
    BOOST_CHECK(models[0]->getBoundingBox()); // shall remain untouched
}

BOOST_AUTO_TEST_CASE(add_instance)
{
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    auto model = models[0];
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
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    auto model = models[0];
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
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    auto model = models[0];
    BOOST_REQUIRE_EQUAL(model->getInstances().size(), 2);

    model->removeInstance(model->getInstances()[1].getInstanceID());
    process();

    BOOST_REQUIRE_EQUAL(model->getInstances().size(), 1);
}

BOOST_AUTO_TEST_CASE(remove_model)
{
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    const auto desc = models[0];

    BOOST_CHECK(
        (makeRequest<size_ts, bool>(REMOVE_MODEL, {desc->getModelID()})));

    BOOST_CHECK_EQUAL(getScene().getModelDescriptors().size(), 0);
}
