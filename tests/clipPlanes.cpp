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

#include "ClientServer.h"

const std::string ADD_CLIP_PLANE("add-clip-plane");
const std::string GET_CLIP_PLANES("get-clip-planes");
const std::string REMOVE_CLIP_PLANES("remove-clip-planes");
const std::string UPDATE_CLIP_PLANE("update-clip-plane");

TEST_CASE_FIXTURE(ClientServer, "add_plane")
{
    REQUIRE(getScene().getClipPlanes().empty());
    const brayns::Plane equation{{1.0, 2.0, 3.0, 4.0}};
    const auto result =
        makeRequest<brayns::Plane, brayns::ClipPlane>(ADD_CLIP_PLANE, equation);
    CHECK_EQ(result.getID(), 0);
    CHECK(result.getPlane() == equation);
    REQUIRE_EQ(getScene().getClipPlanes().size(), 1);
    CHECK(getScene().getClipPlane(0)->getPlane() == equation);
    CHECK(getScene().getClipPlane(1) == brayns::ClipPlanePtr());

    getScene().removeClipPlane(0);
    CHECK(getScene().getClipPlanes().empty());
}

TEST_CASE_FIXTURE(ClientServer, "get_planes")
{
    const brayns::Plane equation1{{1.0, 1.0, 1.0, 1.0}};
    const brayns::Plane equation2{{2.0, 2.0, 2.0, 2.0}};

    const auto id1 = getScene().addClipPlane(equation1);
    const auto id2 = getScene().addClipPlane(equation2);

    const auto result = makeRequest<brayns::ClipPlanes>(GET_CLIP_PLANES);
    CHECK_EQ(result.size(), 2);
    CHECK(result[0]->getPlane() == equation1);
    CHECK(result[1]->getPlane() == equation2);

    getScene().removeClipPlane(id1);
    getScene().removeClipPlane(id2);
}

TEST_CASE_FIXTURE(ClientServer, "update_plane")
{
    Client client(ClientServer::instance());

    const brayns::Plane equation1{{1.0, 1.0, 1.0, 1.0}};
    const brayns::Plane equation2{{2.0, 2.0, 2.0, 2.0}};

    const auto id1 = getScene().addClipPlane(equation1);

    makeRequest<brayns::ClipPlane, bool>(UPDATE_CLIP_PLANE,
                                         brayns::ClipPlane(id1, equation2));

    CHECK(getScene().getClipPlane(id1)->getPlane() == equation2);
    getScene().removeClipPlane(id1);
}

TEST_CASE_FIXTURE(ClientServer, "remove_planes")
{
    const brayns::Plane equation{{1.0, 2.0, 3.0, 4.0}};
    const auto id1 = getScene().addClipPlane(equation);
    const auto id2 = getScene().addClipPlane(equation);
    const auto id3 = getScene().addClipPlane(equation);
    makeRequest<size_ts, bool>(REMOVE_CLIP_PLANES, {id2});
    makeRequest<size_ts, bool>(REMOVE_CLIP_PLANES, {id1, id3});
    CHECK(getScene().getClipPlanes().empty());
}

TEST_CASE_FIXTURE(ClientServer, "notifications")
{
    Client client(ClientServer::instance());

    bool called = false;
    brayns::ClipPlane notified;
    size_ts ids;
    client.client.connect<brayns::ClipPlane>(
        UPDATE_CLIP_PLANE,
        [&notified, &called](const brayns::ClipPlane& plane) {
            notified = plane;
            called = true;
        });
    client.client.connect<size_ts>(REMOVE_CLIP_PLANES,
                                   [&ids, &called](const size_ts& ids_) {
                                       ids = ids_;
                                       called = true;
                                   });
    process();

    auto added =
        makeRequest<brayns::Plane, brayns::ClipPlane>(ADD_CLIP_PLANE,
                                                      {{1.0, 1.0, 1.0, 1.0}});

    process();
    for (size_t attempts = 0; attempts != 100 && !called; ++attempts)
        client.process();
    REQUIRE(called);

    CHECK_EQ(notified.getID(), added.getID());
    CHECK(notified.getPlane() == added.getPlane());

    added.setPlane({{2.0, 2.0, 2.0, 2.0}});
    makeRequest<brayns::ClipPlane, bool>(UPDATE_CLIP_PLANE, added);
    notified = brayns::ClipPlane();

    process();
    called = false;
    for (size_t attempts = 0; attempts != 100 && !called; ++attempts)
        client.process();
    REQUIRE(called);

    CHECK_EQ(notified.getID(), added.getID());
    CHECK(notified.getPlane() == added.getPlane());

    makeRequest<size_ts, bool>(REMOVE_CLIP_PLANES, {added.getID()});

    process();
    called = false;
    for (size_t attempts = 0; attempts != 100 && !called; ++attempts)
        client.process();
    REQUIRE(called);

    CHECK(ids == size_ts{added.getID()});
}
