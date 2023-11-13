/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest/doctest.h>

#include <brayns/engine/components/Volumes.h>
#include <brayns/io/loaders/volume/VolumeLoader.h>
#include <brayns/utils/FileReader.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/paths.h>
#include <tests/unit/PlaceholderEngine.h>

#include <filesystem>

namespace
{
class VolumeExtractor
{
public:
    static const brayns::RegularVolume &extract(const brayns::Model &model)
    {
        auto &components = model.getComponents();
        auto volumes = components.find<brayns::Volumes>();
        CHECK(volumes);
        auto &volume = volumes->elements.front();
        auto cast = volume.as<brayns::RegularVolume>();
        CHECK(cast);
        return *cast;
    }
};
}

TEST_CASE("Raw Volume loader")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Name")
    {
        auto loader = brayns::RawVolumeLoader();
        CHECK(loader.getName() == "raw-volume");
    }
    SUBCASE("Invalid load")
    {
        auto loader = brayns::RawVolumeLoader();

        auto invalidDimensions = brayns::RawVolumeLoader::FileRequest();
        invalidDimensions.path = TestPaths::Volumes::raw;
        invalidDimensions.params.data_type = brayns::VolumeDataType::Float;
        invalidDimensions.params.dimensions = brayns::Vector3ui(0);
        invalidDimensions.params.spacing = brayns::Vector3f(1.f);
        CHECK_THROWS_WITH(loader.loadFile(invalidDimensions), "Volume dimensions are empty");

        auto invalidDataType = brayns::RawVolumeLoader::FileRequest();
        invalidDataType.path = TestPaths::Volumes::raw;
        invalidDataType.params.data_type = brayns::VolumeDataType::Short;
        invalidDataType.params.dimensions = brayns::Vector3ui(256, 256, 112);
        invalidDataType.params.spacing = brayns::Vector3f(1.f);
        CHECK_THROWS_WITH(loader.loadFile(invalidDataType), "Data size and expected size mismatch");

        auto request = brayns::RawVolumeLoader::FileRequest();
        request.path = TestPaths::Volumes::emptyRaw;
        request.params.data_type = brayns::VolumeDataType::Float;
        request.params.dimensions = brayns::Vector3ui(256, 256, 112);
        request.params.spacing = brayns::Vector3f(1.f);
        CHECK_THROWS_WITH(loader.loadFile(request), "Data size and expected size mismatch");
    }
    SUBCASE("Load")
    {
        auto loader = brayns::RawVolumeLoader();

        auto request = brayns::RawVolumeLoader::FileRequest();
        request.path = TestPaths::Volumes::raw;
        request.params.data_type = brayns::VolumeDataType::Float;
        request.params.dimensions = brayns::Vector3ui(256, 256, 112);
        request.params.spacing = brayns::Vector3f(1.f);
        auto result = loader.loadFile(request);

        CHECK(result.size() == 1);

        auto &model = *result.front();
        auto &volume = VolumeExtractor::extract(model);
        CHECK(volume.dataType == request.params.data_type);
        CHECK(volume.size == request.params.dimensions);
        CHECK(volume.spacing == request.params.spacing);
    }
}

TEST_CASE("Mhd Volume loader")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Name")
    {
        auto loader = brayns::MHDVolumeLoader();
        CHECK(loader.getName() == "mhd-volume");
    }
    SUBCASE("Invalid load")
    {
        auto loader = brayns::MHDVolumeLoader();

        auto request = brayns::MHDVolumeLoader::FileRequest();

        request.path = TestPaths::Volumes::emptyMhd;
        CHECK_THROWS_WITH(loader.loadFile(request), "Incomplete MHD file. Missing ObjectType");

        request.path = TestPaths::Volumes::invalidMhd;
        CHECK_THROWS_WITH(loader.loadFile(request), "Incomplete MHD file. Missing ElementType");

        request.path = TestPaths::Volumes::badFormatMhd;
        CHECK_THROWS_WITH(loader.loadFile(request), "Unsupported data type: MET_INT");

        request.path = TestPaths::Volumes::badObjectTypeMhd;
        CHECK_THROWS_WITH(loader.loadFile(request), "Wrong object type for mhd file");
    }
    SUBCASE("Load")
    {
        auto loader = brayns::MHDVolumeLoader();

        auto request = brayns::MHDVolumeLoader::FileRequest();
        request.path = TestPaths::Volumes::mhd;

        auto result = loader.loadFile(request);
        CHECK(result.size() == 1);

        auto &model = *result.front();
        auto &volume = VolumeExtractor::extract(model);
        CHECK(volume.dataType == brayns::VolumeDataType::Float);
        CHECK(volume.size == brayns::Vector3ui(256, 256, 112));
        CHECK(volume.spacing == brayns::Vector3f(1.16f, 1.16f, 2.5f));
    }
}
