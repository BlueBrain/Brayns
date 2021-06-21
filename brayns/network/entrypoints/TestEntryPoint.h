#pragma once

#include "base/BasicEntryPoint.h"

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(TestRequest)
BRAYNS_MESSAGE_ENTRY(int, test, "Test int")
BRAYNS_MESSAGE_END()

BRAYNS_MESSAGE_BEGIN(TestReply)
BRAYNS_MESSAGE_ENTRY(std::string, test, "Test string")
BRAYNS_MESSAGE_END()

class TestEntryPoint : public BasicEntryPoint<TestRequest, TestReply>
{
public:
    TestEntryPoint()
    {
        setName("test");
        setDescription("This is a test");
    }

    virtual TestReply run(const TestRequest& request) const override
    {
        TestReply reply;
        reply.test = "Success";
        return reply;
    }
};
} // namespace brayns