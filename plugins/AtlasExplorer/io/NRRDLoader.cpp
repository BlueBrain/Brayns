/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "nrrdloader/data/DataParser.h"
#include "nrrdloader/header/HeaderParser.h"
#include "nrrdloader/header/HeaderUtils.h"

#include <brayns/utils/FileReader.h>

#include <api/AtlasFactory.h>
#include <api/usecases/OutlineShell.h>
#include <components/AtlasData.h>

std::vector<std::string> NRRDLoader::getSupportedExtensions() const
{
    return {".nrrd"};
}

std::string NRRDLoader::getName() const
{
    return "NRRD loader";
}

std::vector<std::shared_ptr<brayns::Model>> NRRDLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &callback,
    const NRRDLoaderParameters &parameters) const
{
    (void)blob;
    (void)callback;
    (void)parameters;
    throw std::runtime_error("Binary load is not allowed for NRRD files");
}

std::vector<std::shared_ptr<brayns::Model>> NRRDLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const NRRDLoaderParameters &parameters) const
{
    auto fileContent = brayns::FileReader::read(path);
    auto contentView = std::string_view(fileContent);

    auto header = HeaderParser::parse(path, contentView);
    auto size = HeaderUtils::get3DSize(header);
    auto spacing = HeaderUtils::get3DDimensions(header);
    auto transform = HeaderUtils::getTransform(header);

    callback.updateProgress("Parsing NRRD data", 0.4f);
    auto data = DataParser::parse(header, contentView);

    callback.updateProgress("Transforming data", 0.6f);
    auto factory = AtlasFactory::createDefault();
    auto atlas = factory.create(parameters.type, size, spacing, *data);

    callback.updateProgress("Generating volume mesh", 0.8f);
    auto model = OutlineShell().run(*atlas, {});
    auto &components = model->getComponents();
    components.add<AtlasData>(std::move(atlas));
    components.add<brayns::Transform>(transform);

    callback.updateProgress("Done", 1.f);
    auto result = std::vector<std::shared_ptr<brayns::Model>>();
    result.push_back(std::move(model));
    return result;
}
