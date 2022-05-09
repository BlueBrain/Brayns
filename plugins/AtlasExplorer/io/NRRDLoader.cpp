/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "NRRDLoader.h"

#include <brayns/utils/EnumUtils.h>
#include <brayns/utils/FileReader.h>

#include <io/nrrdloader/header/HeaderLimitCheck.h>
#include <io/nrrdloader/header/HeaderParser.h>

std::vector<std::string> NRRDLoader::getSupportedExtensions() const
{
    return {".nrrd"};
}

std::string NRRDLoader::getName() const
{
    return "NRRD loader";
}

std::vector<std::unique_ptr<brayns::Model>> NRRDLoader::importFromBlob(
    brayns::Blob &&blob,
    const brayns::LoaderProgress &callback) const
{
    (void)blob;
    (void)callback;
    throw std::runtime_error("NRRD Loader does not support blob loading");
}

std::vector<std::unique_ptr<brayns::Model>> NRRDLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback) const
{
    callback.updateProgress("Reading " + path, 0.f);
    const auto fileContent = brayns::FileReader::read(path);
    auto contentView = std::string_view(fileContent);

    callback.updateProgress("Parsing NRRD header", 0.2f);
    const auto header = HeaderParser::parse(contentView);
    // Check the header against the limits imposed by what this plugin can render
    HeaderLimitCheck::check(header);

    auto model = std::make_unique<brayns::Model>();
    auto result = std::vector<std::unique_ptr<brayns::Model>>();
    result.push_back(std::move(model));
    return result;
}
