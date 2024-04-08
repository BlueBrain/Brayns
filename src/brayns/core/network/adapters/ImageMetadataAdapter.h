/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/core/json/Json.h>

#include <brayns/core/utils/image/ImageMetadata.h>

namespace brayns
{
template<>
struct JsonAdapter<ImageMetadata> : ObjectAdapter<ImageMetadata>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ImageMetadata");
        builder
            .getset(
                "title",
                [](auto &object) { return object.title; },
                [](auto &object, auto value) { object.title = std::move(value); })
            .description("Image title");
        builder
            .getset(
                "description",
                [](auto &object) -> auto & { return object.description; },
                [](auto &object, auto value) { object.description = std::move(value); })
            .description("Image description");
        builder
            .getset(
                "where_used",
                [](auto &object) -> auto & { return object.whereUsed; },
                [](auto &object, auto value) { object.whereUsed = std::move(value); })
            .description("Event (publication, website, panel, etc.) for what the image was created");
        builder
            .getset(
                "keywords",
                [](auto &object) -> auto & { return object.keywords; },
                [](auto &object, auto value) { object.keywords = std::move(value); })
            .description("List of keywords to describe the image contents");
        return builder.build();
    }
};
}
