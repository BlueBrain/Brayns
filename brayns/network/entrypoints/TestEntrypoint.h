/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#pragma once

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
enum class TestEnum
{
    Test1,
    Test2,
    Test3
};

BRAYNS_ADAPTER_ENUM(TestEnum,
        {"Test1", TestEnum::Test1},
        {"Test2", TestEnum::Test2},
        {"Test3", TestEnum::Test3})

BRAYNS_MESSAGE_BEGIN(TestParams)
BRAYNS_MESSAGE_ENTRY(int, test, "Test int")
BRAYNS_MESSAGE_ENTRY(Vector3d, vec, "Test vec3")
BRAYNS_MESSAGE_ENTRY(Quaterniond, qua, "Test qua")
BRAYNS_MESSAGE_ENTRY(std::vector<std::string>, vector, "Test vector")
BRAYNS_MESSAGE_ENTRY(std::list<std::string>, list, "Test list")
BRAYNS_MESSAGE_OPTION(std::shared_ptr<std::string>, stringptr, "Test ptr")
BRAYNS_MESSAGE_OPTION(TestEnum, enumeration, "Test enum")
BRAYNS_MESSAGE_END()

BRAYNS_MESSAGE_BEGIN(TestResult)
BRAYNS_MESSAGE_ENTRY(std::string, test, "Test string")
BRAYNS_MESSAGE_ENTRY(size_t, size, "Test size")
BRAYNS_MESSAGE_ENTRY(StringMap<TestParams>, map, "Test map")
BRAYNS_MESSAGE_ENTRY(StringHash<std::string>, hash, "Test hash")
BRAYNS_MESSAGE_ENTRY(std::shared_ptr<int>, intptr, "Test empty ptr",
                     WriteOnly())
BRAYNS_MESSAGE_END()

class TestEntrypoint : public Entrypoint<TestParams, TestResult>
{
public:
    virtual std::string getName() const override { return "test"; }

    virtual std::string getDescription() const override { return "Test"; }

    virtual void onRequest(const Request& request) override
    {
        auto& params = request.getParams();
        request.progress("This is a 0% progress", 0.0);
        TestResult result;
        result.test = "Success";
        notify(std::string("This is a notification"));
        result.size = params.vector.size();
        if (result.size == 0)
        {
            throw EntrypointException("This is an error");
        }
        result.intptr = std::make_shared<int>(0);
        request.progress("This is a 100% progress", 1.0);
        request.reply(result);
    }
};
} // namespace brayns