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

#include <doctest/doctest.h>

#include <tests/paths.h>

TEST_CASE("File reader")
{
    CHECK_THROWS_WITH(brayns::FileReader::read("/fake/file.a"), "File /fake/file.a does not exists");
    CHECK_THROWS_WITH(
        brayns::FileReader::read(TestPaths::Folders::testFiles),
        doctest::Contains("is not a regular file"));
    CHECK_NOTHROW(brayns::FileReader::read(TestPaths::Meshes::obj));
}
