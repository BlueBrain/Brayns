/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

        auto invalidDimensions = brayns::RawVolumeLoaderParameters();
        invalidDimensions.data_type = brayns::VolumeDataType::Float;
        invalidDimensions.dimensions = brayns::Vector3ui(0);
        invalidDimensions.spacing = brayns::Vector3f(1.f);
        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::raw, {}, invalidDimensions),
            "Volume dimensions are empty");

        auto invalidDataType = brayns::RawVolumeLoaderParameters();
        invalidDataType.data_type = brayns::VolumeDataType::Short;
        invalidDataType.dimensions = brayns::Vector3ui(256, 256, 112);
        invalidDataType.spacing = brayns::Vector3f(1.f);
        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::raw, {}, invalidDimensions),
            "Data size and exptected size mismatch");

        auto params = brayns::RawVolumeLoaderParameters();
        params.data_type = brayns::VolumeDataType::Float;
        params.dimensions = brayns::Vector3ui(256, 256, 112);
        params.spacing = brayns::Vector3f(1.f);
        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::emptyRaw, {}, params),
            "Data size and exptected size mismatch");
    }
    SUBCASE("Load")
    {
        auto loader = brayns::RawVolumeLoader();

        auto params = brayns::RawVolumeLoaderParameters();
        params.data_type = brayns::VolumeDataType::Float;
        params.dimensions = brayns::Vector3ui(256, 256, 112);
        params.spacing = brayns::Vector3f(1.f);
        auto result = loader.importFromFile(TestPaths::Volumes::raw, {}, params);
        CHECK(result.size() == 1);

        auto &model = *result.front();
        auto &volume = VolumeExtractor::extract(model);
        CHECK(volume.dataType == params.data_type);
        CHECK(volume.size == params.dimensions);
        CHECK(volume.spacing == params.spacing);
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

        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::emptyMhd, {}),
            "Incomplete MHD file. Missing ObjectType");

        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::invalidMhd, {}),
            "Incomplete MHD file. Missing ElementType");

        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::badFormatMhd, {}),
            "Unsupported data type: MET_INT");

        CHECK_THROWS_MESSAGE(
            loader.importFromFile(TestPaths::Volumes::badObjectTypeMhd, {}),
            "Wrong object type for mhd file");
    }
    SUBCASE("Load")
    {
        auto loader = brayns::MHDVolumeLoader();

        auto result = loader.importFromFile(TestPaths::Volumes::mhd, {});
        CHECK(result.size() == 1);

        auto &model = *result.front();
        auto &volume = VolumeExtractor::extract(model);
        CHECK(volume.dataType == brayns::VolumeDataType::Float);
        CHECK(volume.size == brayns::Vector3ui(256, 256, 112));
        CHECK(volume.spacing == brayns::Vector3f(1.16f, 1.16f, 2.5f));
    }
}
