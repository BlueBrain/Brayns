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

#include "DataReader.h"

#include <brayns/utils/FileReader.h>

#include <filesystem>

namespace
{
class DataFilePathFixer
{
public:
    static std::vector<std::string> createFixed(const NRRDHeader &header, const std::string &fileName)
    {
        const auto filePath = std::filesystem::path(fileName);
        const auto basePath = filePath.parent_path();

        // Copy, manipulate, return
        auto fileList = *header.dataFiles;

        for (auto &file : fileList)
        {
            const auto filePath = basePath / std::filesystem::path(file);
            file = filePath.string();
        }

        return fileList;
    }
};
class DataFileReader
{
public:
    static std::vector<uint8_t> read(const NRRDHeader &header, const std::string &fileName)
    {
        auto fileList = DataFilePathFixer::createFixed(header, fileName);

        for (const auto &file : fileList)
        {
            auto content = brayns::FileReader::read(file);
        }
    }
};
}

std::vector<uint8_t>
    DataReader::read(const NRRDHeader &header, const std::string &fileName, std::string_view dataContent)
{
}
