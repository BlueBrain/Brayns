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

#include <brayns/json/Json.h>

#include <brayns/engine/components/LoadInfo.h>

namespace brayns
{
template<>
struct EnumReflector<LoadInfo::LoadSource>
{
    static EnumMap<LoadInfo::LoadSource> reflect()
    {
        return {
            {"from_file", LoadInfo::LoadSource::FromFile},
            {"from_blob", LoadInfo::LoadSource::FromBlob},
            {"none", LoadInfo::LoadSource::None}};
    }
};

template<>
struct JsonAdapter<LoadInfo::LoadSource> : EnumAdapter<LoadInfo::LoadSource>
{
};

template<>
struct JsonAdapter<LoadInfo> : ObjectAdapter<LoadInfo>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("LoadInfo");
        builder.get("source", [](auto &object) { return object.source; }).description("Model load source");
        builder
            .get(
                "path",
                [](auto &object) -> auto & { return object.path; })
            .description("File path in case of file load type");
        builder
            .get(
                "loader_name",
                [](auto &object) -> auto & { return object.loaderName; })
            .description("Loader name");
        builder
            .get(
                "load_parameters",
                [](auto &object) -> auto & { return object.loadParameters; })
            .description("Loader settings");
        return builder.build();
    }
};
} // namespace brayns
