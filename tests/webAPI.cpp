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

#define BOOST_TEST_MODULE braynsWebAPI

#include "jsonSerialization.h"

#include "ClientServer.h"
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/Renderer.h>

#include "ImageGenerator.h"

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(reset_camera)
{
    const auto target = getCamera().getTarget();
    getCamera().setTarget({1, 2, 3});
    makeNotification("reset-camera");
    BOOST_CHECK_EQUAL(getCamera().getTarget(), target);
}

BOOST_AUTO_TEST_CASE(inspect)
{
    auto inspectResult =
        makeRequest<std::array<float, 2>, brayns::Renderer::PickResult>(
            "inspect", {{0.5, 0.5}});
    BOOST_CHECK(inspectResult.hit);
    BOOST_CHECK(inspectResult.pos.equals(
        {0.500001490116119, 0.500001490116119, 1.19209289550781e-7}));

    auto failedInspectResult =
        makeRequest<std::array<float, 2>, brayns::Renderer::PickResult>(
            "inspect", {{10, -10}});
    BOOST_CHECK(!failedInspectResult.hit);
}

#ifdef BRAYNS_USE_MAGICKPP
BOOST_AUTO_TEST_CASE(snapshot)
{
    brayns::SnapshotParams params;
    params.format = "jpg";
    params.size = {5, 5};
    params.quality = 75;

    auto image =
        makeRequest<brayns::SnapshotParams,
                    brayns::ImageGenerator::ImageBase64>("snapshot", params);
    BOOST_CHECK_EQUAL(image.data,
                      "/9j/4AAQSkZJRgABAQAAAQABAAD/"
                      "2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4n"
                      "ICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/"
                      "2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIy"
                      "MjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/"
                      "wAARCAAFAAUDASIAAhEBAxEB/8QAFQABAQAAAAAAAAAAAAAAAAAAAAX/"
                      "xAAgEAABAwMFAQAAAAAAAAAAAAACAAEEAwURBxIhMkGB/"
                      "8QAFQEBAQAAAAAAAAAAAAAAAAAABAb/"
                      "xAAcEQACAgIDAAAAAAAAAAAAAAABAgADBEEFEdH/2gAMAwEAAhEDEQA/"
                      "AJ0PVMbfBjwxsrmMekFJiKU3O0WHPT3GfqIir6OLxGqUlNDZ9hVsboT/"
                      "2Q==");
}
#endif

BOOST_AUTO_TEST_CASE(snapshot_empty_params)
{
    BOOST_CHECK_THROW((makeRequest<brayns::SnapshotParams,
                                   brayns::ImageGenerator::ImageBase64>(
                          "snapshot", brayns::SnapshotParams())),
                      rockets::jsonrpc::response_error);
}

BOOST_AUTO_TEST_CASE(snapshot_illegal_format)
{
    brayns::SnapshotParams params;
    params.size = {5, 5};
    params.format = "";
    BOOST_CHECK_THROW(
        (makeRequest<brayns::SnapshotParams,
                     brayns::ImageGenerator::ImageBase64>("snapshot", params)),
        rockets::jsonrpc::response_error);
}
