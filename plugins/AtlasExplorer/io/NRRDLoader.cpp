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

#include <brayns/utils/FileReader.h>

#include <api/usecases/OutlineShell.h>
#include <components/AtlasComponent.h>
#include <io/nrrdloader/data/DataParser.h>
#include <io/nrrdloader/header/HeaderLimitCheck.h>
#include <io/nrrdloader/header/HeaderParser.h>
#include <io/nrrdloader/header/HeaderUtils.h>

std::vector<std::string> NRRDLoader::getSupportedExtensions() const
{
    return {".nrrd"};
}

std::string NRRDLoader::getName() const
{
    return "NRRD loader";
}

std::vector<std::unique_ptr<brayns::Model>> NRRDLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &callback) const
{
    auto &bytes = blob.data;
    auto tempCast = static_cast<const void *>(bytes.data());
    auto charCast = static_cast<const char *>(tempCast);
    auto contentView = std::string_view(charCast, bytes.size());

    callback.updateProgress("Parsing NRRD header", 0.2f);
    auto header = HeaderParser::parse(blob.name, contentView);
    HeaderLimitCheck::check(header);

    callback.updateProgress("Parsing NRRD data", 0.4f);
    auto data = DataParser::parse(header, contentView);

    callback.updateProgress("Transforming data", 0.6f);
    const auto voxelSize = HeaderUtils::getVoxelDimension(header);
    const auto gridSize = HeaderUtils::get3DSize(header);
    const auto gridSpacing = HeaderUtils::get3DDimensions(header);
    auto atlasVolume = std::make_shared<AtlasVolume>(gridSize, gridSpacing, voxelSize, std::move(data));

    callback.updateProgress("Generating volume mesh", 0.8f);
    auto model = OutlineShell().execute(*atlasVolume, {});
    model->addComponent<AtlasComponent>(atlasVolume);

    callback.updateProgress("Done", 1.f);
    auto result = std::vector<std::unique_ptr<brayns::Model>>();
    result.push_back(std::move(model));
    return result;
}

std::vector<std::unique_ptr<brayns::Model>> NRRDLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback) const
{
    callback.updateProgress("Reading " + path, 0.f);
    const auto fileContent = brayns::FileReader::read(path);

    brayns::Blob blob;
    blob.type = "nrrd";
    blob.name = path;
    blob.data = std::vector<uint8_t>(fileContent.begin(), fileContent.end());

    return importFromBlob(blob, callback);
}
