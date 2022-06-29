#pragma once

#include <brayns/common/MathTypes.h>

#include <vector>

namespace dti
{
struct StreamlineData
{
    size_t linealIndex{};
    std::vector<brayns::Vector3f> points;
};
}
