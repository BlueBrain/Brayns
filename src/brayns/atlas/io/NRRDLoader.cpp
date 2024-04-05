/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/utils/FileReader.h>

#include <brayns/atlas/api/AtlasFactory.h>
#include <brayns/atlas/api/usecases/OutlineShell.h>
#include <brayns/atlas/components/AtlasData.h>

std::string NRRDLoader::getName() const
{
    return "NRRD loader";
}

std::vector<std::string> NRRDLoader::getExtensions() const
{
    return {".nrrd"};
}

std::vector<std::shared_ptr<brayns::Model>> NRRDLoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);
    auto &progress = request.progress;

    auto fileContent = brayns::FileReader::read(path);
    auto contentView = std::string_view(fileContent);

    auto header = HeaderParser::parse(path, contentView);
    auto size = HeaderUtils::get3DSize(header);
    auto spacing = HeaderUtils::get3DDimensions(header);
    auto transform = HeaderUtils::getTransform(header);

    progress("Parsing NRRD data", 0.4f);
    auto data = DataParser::parse(header, contentView);

    progress("Transforming data", 0.6f);
    auto factory = AtlasFactory::createDefault();
    auto atlas = factory.create(request.params.type, size, spacing, *data);

    progress("Generating volume mesh", 0.8f);
    auto model = OutlineShell().run(*atlas, {});
    auto &components = model->getComponents();
    components.add<AtlasData>(std::move(atlas));
    components.add<brayns::Transform>(transform);

    progress("Done", 1.f);
    auto result = std::vector<std::shared_ptr<brayns::Model>>();
    result.push_back(std::move(model));
    return result;
}
