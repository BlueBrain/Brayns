#include "HeaderUtils.h"

brayns::Vector3ui HeaderUtils::get3DSize(const NRRDHeader &header)
{
    const auto &sizes = header.sizes;
    assert(sizes.size() >= 3);

    brayns::Vector3ui result;
    const auto start = sizes.size() - 3;
    for (size_t i = start; i < sizes.size(); ++i)
    {
        result[i - start] = sizes[i];
    }
    return result;
}

brayns::Vector3f HeaderUtils::get3DDimensions(const NRRDHeader &header)
{
    brayns::Vector3f result(1.f);

    const auto &spaceDirections = header.spaceDirections;
    if (!spaceDirections)
    {
        return result;
    }

    const auto &directions = *spaceDirections;
    assert(directions.size() == 3);

    for (size_t i = 0; i < directions.size(); ++i)
    {
        const auto &direction = directions[i];
        brayns::Vector3f vector;
        for (size_t j = 0; j < 3; ++j)
        {
            vector[j] = direction[j];
        }

        result[i] = glm::length(vector);
    }
    return result;
}

size_t HeaderUtils::getVoxelDimension(const NRRDHeader &header)
{
    if (header.dimensions <= 3)
    {
        return 1;
    }

    return header.sizes.front();
}
