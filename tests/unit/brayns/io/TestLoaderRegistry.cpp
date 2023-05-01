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
class MockLoader final : public brayns::Loader<MockParameters>
{
public:
    static inline const std::string name = "mock-loader";
    static inline const std::vector<std::string> extensions = {".a"};

    std::string getName() const override
    {
        return name;
    }

    std::vector<std::string> getSupportedExtensions() const override
    {
        return extensions;
    }

    std::vector<std::shared_ptr<brayns::Model>> importFromBlob(
        const brayns::Blob &blob,
        const brayns::LoaderProgress &callback,
        const MockParameters &params) const override
    {
        (void)blob;
        (void)callback;
        (void)params;
        return {};
    }

    std::vector<std::shared_ptr<brayns::Model>> importFromFile(
        const std::string &path,
        const brayns::LoaderProgress &callback,
        const MockParameters &params) const override
    {
        (void)path;
        (void)callback;
        (void)params;
        return {};
    }
};

class MockNoInputLoader final : public brayns::NoInputLoader
{
public:
    static inline const std::string name = "mock-noinput-loader";
    static inline const std::vector<std::string> extensions = {".b"};

    std::string getName() const override
    {
        return name;
    }

    std::vector<std::string> getSupportedExtensions() const override
    {
        return extensions;
    }

    std::vector<std::shared_ptr<brayns::Model>> importFromBlob(
        const brayns::Blob &blob,
        const brayns::LoaderProgress &callback) const override
    {
        (void)blob;
        (void)callback;
        return {};
    }

    std::vector<std::shared_ptr<brayns::Model>> importFromFile(
        const std::string &path,
        const brayns::LoaderProgress &callback) const override
    {
        (void)path;
        (void)callback;
        return {};
    }
};
} // namespace

TEST_CASE("Loader registry")
{
    SUBCASE("Register loader")
    {
        auto registry = brayns::LoaderRegistry();
        CHECK(registry.getLoaderInfos().empty());

        registry.registerLoader(std::make_unique<MockLoader>());
        registry.registerLoader(std::make_unique<MockNoInputLoader>());
        auto &infos = registry.getLoaderInfos();
        CHECK(infos.size() == 2);
        CHECK(infos[0].name == MockLoader::name);
        CHECK(infos[0].extensions == MockLoader::extensions);
        CHECK(infos[1].name == MockNoInputLoader::name);
        CHECK(infos[1].extensions == MockNoInputLoader::extensions);

        registry.clear();
        CHECK(registry.getLoaderInfos().empty());
    }
    SUBCASE("Query suitability")
    {
        auto registry = brayns::LoaderRegistry();
        registry.registerLoader(std::make_unique<MockLoader>());
        registry.registerLoader(std::make_unique<MockNoInputLoader>());

        CHECK(registry.isSupportedFile("/a/file/path.a"));
        CHECK(registry.isSupportedFile("/another/file/path.b"));
        CHECK(!registry.isSupportedFile("/a/third/file/path.c"));
        CHECK(!registry.isSupportedFile("/and/why/not/a/forth"));
        CHECK(registry.isSupportedFile("even_local_files.a"));
        CHECK(registry.isSupportedFile("and/relatives/too.b"));

        CHECK(registry.isSupportedType("a"));
        CHECK(registry.isSupportedType(".a"));
        CHECK(registry.isSupportedType("b"));
        CHECK(!registry.isSupportedType("c"));
        CHECK(!registry.isSupportedType(".c"));
        CHECK(!registry.isSupportedType(""));
    }
    SUBCASE("Retrieve loader")
    {
        auto registry = brayns::LoaderRegistry();
        registry.registerLoader(std::make_unique<MockLoader>());

        CHECK_NOTHROW(registry.getSuitableLoader("/a/path.a", "", ""));
        CHECK_NOTHROW(registry.getSuitableLoader("", ".a", ""));
        CHECK_NOTHROW(registry.getSuitableLoader("", "", "mock-loader"));
        CHECK_NOTHROW(registry.getSuitableLoader("path.a", "a", ""));
        CHECK_NOTHROW(registry.getSuitableLoader("another/path.a", "", "mock-loader"));
        CHECK_NOTHROW(registry.getSuitableLoader("", ".a", "mock-loader"));
        CHECK_NOTHROW(registry.getSuitableLoader("/final/path.a", ".a", "mock-loader"));

        CHECK_THROWS_AS(registry.getSuitableLoader("", "", ""), std::runtime_error);
        CHECK_THROWS_AS(registry.getSuitableLoader("/wrong/path.c", "", ""), std::runtime_error);
        CHECK_THROWS_AS(registry.getSuitableLoader("", ".badextension", ""), std::runtime_error);
        CHECK_THROWS_AS(registry.getSuitableLoader("", "", "bad-loader-name"), std::runtime_error);
        CHECK_THROWS_AS(registry.getSuitableLoader("", ".a", "bad-loader-name"), std::runtime_error);
        CHECK_THROWS_AS(registry.getSuitableLoader("a/path.a", "", "bad-loader-name"), std::runtime_error);
    }
}
