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

const std::string UPDATE_MODEL("update-model");
const std::string REMOVE_MODEL("remove-model");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(update_model)
{
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    const uint32_t id = models[0]->getID();

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

BOOST_AUTO_TEST_CASE(remove_model)
{
    const auto& models = getScene().getModelDescriptors();
    BOOST_REQUIRE_EQUAL(models.size(), 1);

    const auto desc = models[0];

    BOOST_CHECK((makeRequest<size_ts, bool>(REMOVE_MODEL, {desc->getID()})));

    BOOST_CHECK_EQUAL(getScene().getModelDescriptors().size(), 0);
}
