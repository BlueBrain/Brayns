#ifndef DTI_TYPES_H
#define DTI_TYPES_H

#include <brayns/common/Transformation.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>
#include <string>

using Matrix3f = glm::mat<3, 3, float>;
using Quaternionf = glm::tquat<float, glm::highp>;

/** Color scheme */
enum class ColorScheme
{
    none = 0,
    by_id = 1,
    by_normal = 2
};

struct DTIConfiguration
{
    std::string streamlines;
    std::string gid_to_streamline;
};

using Point = brayns::Vector3f;
using Points = std::vector<Point>;
using Colors = brayns::Vector4fs;
using Streamlines = std::map<uint64_t, Points>;
using Indices = std::vector<size_t>;
using Vertices = std::vector<float>;
using Gids = std::vector<uint64_t>;
using Transformations = std::vector<brayns::Transformation>;

#endif // DTI_TYPES_H
