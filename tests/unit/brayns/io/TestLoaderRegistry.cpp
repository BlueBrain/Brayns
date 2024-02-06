/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/io/Loader.h>
#include <brayns/io/LoaderFormat.h>
#include <brayns/io/LoaderRegistry.h>

#include <brayns/json/Json.h>

namespace
{
struct MockParameters
{
    std::string stringParam;
    int intParam = 0;
};
} // namespace

namespace brayns
{
template<>
struct JsonAdapter<MockParameters> : ObjectAdapter<MockParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("MockParameters");
        builder
            .getset(
                "stringParam",
                [](auto &object) -> auto & { return object.stringParam; },
                [](auto &object, auto value) { object.stringParam = std::move(value); })
            .description("A string parameter");
        builder
            .getset(
                "intParam",
                [](auto &object) { return object.intParam; },
                [](auto &object, auto value) { object.intParam = value; })
            .description("An integer parameter");
        return builder.build();
    }
};
} // namespace brayns

namespace
{
class MockLoader : public brayns::Loader<MockParameters>
{
public:
    static inline const std::string name = "mock-loader";
    static inline const std::vector<std::string> extensions = {"a"};

    std::string getName() const override
    {
        return name;
    }

    std::vector<std::string> getExtensions() const override
    {
        return extensions;
    }

    bool canLoadBinary() const override
    {
        return true;
    }

    std::vector<std::shared_ptr<brayns::Model>> loadBinary(const BinaryRequest &request) override
    {
        (void)request;
        return {};
    }

    std::vector<std::shared_ptr<brayns::Model>> loadFile(const FileRequest &request) override
    {
        (void)request;
        return {};
    }
};
} // namespace

TEST_CASE("Loader registry")
{
    SUBCASE("Register loader")
    {
        auto registry = brayns::LoaderRegistry();
        CHECK(registry.getInfos().empty());

        auto loaders = brayns::LoaderRegistryBuilder("Plugin", registry);

        loaders.add<MockLoader>();

        auto infos = registry.getInfos();

        CHECK(infos.size() == 1);
        CHECK(infos[0].plugin == "Plugin");
        CHECK(infos[0].name == MockLoader::name);
        CHECK(infos[0].extensions == MockLoader::extensions);
        CHECK(infos[0].binary);

        CHECK(infos[0].schema == brayns::Json::getSchema<MockParameters>());
    }
    SUBCASE("Query suitability")
    {
        auto registry = brayns::LoaderRegistry();
        registry.add("", std::make_unique<MockLoader>());

        CHECK(registry.findByName(MockLoader::name));
        CHECK_FALSE(registry.findByName("something wrong"));

        auto valid = std::vector<std::string>({"/a/file/path.a", "local_file.a", "otherfile.A", "some/a"});

        for (const auto &path : valid)
        {
            auto format = brayns::LoaderFormat::fromPath(path);
            CHECK(registry.findByFormat(format));
        }

        auto invalid = std::vector<std::string>(
            {"/another/file/path.b",
             "/a/third/file/path.c",
             "/and/why/not/a/forth",
             "and/relatives/too.b",
             "something"});

        for (const auto &path : invalid)
        {
            auto format = brayns::LoaderFormat::fromPath(path);
            CHECK_FALSE(registry.findByFormat(format));
        }
    }
}
