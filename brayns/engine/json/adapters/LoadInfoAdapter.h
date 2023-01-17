/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapterMacro.h>

#include <brayns/engine/components/LoadInfo.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(
    LoadInfo::LoadSource,
    {"from_file", LoadInfo::LoadSource::FromFile},
    {"from_blob", LoadInfo::LoadSource::FromBlob},
    {"none", LoadInfo::LoadSource::None})

BRAYNS_JSON_ADAPTER_BEGIN(LoadInfo)
BRAYNS_JSON_ADAPTER_ENTRY(source, "Model load source")
BRAYNS_JSON_ADAPTER_ENTRY(path, "File path in case of file load type")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("loader_name", loaderName, "Loader used")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("load_parameters", loadParameters, "Loader configuration")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
