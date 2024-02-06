/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/utils/FileReader.h>
#include <brayns/utils/FileWriter.h>

#include <doctest/doctest.h>

#include <tests/helpers/TemporaryFilename.h>
#include <tests/paths.h>

TEST_CASE("File writer")
{
    CHECK_THROWS(brayns::FileWriter::write("random", TemporaryFilename::generateFake()));

    auto file = TemporaryFilename::generateValid();
    auto content = std::string("this goes/ninside the file/n");
    CHECK_NOTHROW(brayns::FileWriter::write(content, file));

    std::string readContent;
    CHECK_NOTHROW(readContent = brayns::FileReader::read(file));
    CHECK(readContent == content);
}
