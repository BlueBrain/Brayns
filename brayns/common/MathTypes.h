/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>

namespace brayns
{
template <class T>
class Box
{
public:
    using vec = glm::vec<3, T>;

    Box() = default;

    Box(const vec& pMin, const vec& pMax)
        : _min(glm::min(pMin, pMax))
        , _max(glm::max(pMin, pMax))
    {
    }
    inline bool operator==(const Box<T>& other) const
    {
        return _min == other._min && _max == other._max;
    }

    inline void merge(const Box<T>& aabb)
    {
        _min = glm::min(_min, aabb.getMin());
        _max = glm::max(_max, aabb.getMax());
    }

    inline void merge(const vec& point)
    {
        _min = glm::min(_min, point);
        _max = glm::max(_max, point);
    }

    inline void intersect(const Box<T>& aabb)
    {
        _min = glm::max(_min, aabb.getMin());
        _max = glm::min(_max, aabb.getMax());
    }

    inline void reset()
    {
        _min = vec(std::numeric_limits<T>::max());
        _max = vec(-std::numeric_limits<T>::max());
    }

    inline bool isEmpty() const
    {
        return _min.x >= _max.x || _min.y >= _max.y || _min.z >= _max.z;
    }

    inline vec getCenter() const { return (_min + _max) * .5; }
    inline vec getSize() const { return _max - _min; }
    inline const vec& getMin() const { return _min; }
    inline const vec& getMax() const { return _max; }
    inline void setMin(const vec& min) { _min = min; }
    inline void setMax(const vec& max) { _max = max; }

#ifdef __INTEL_COMPILER // Workaround for ICC. Make members public
public:
    vec _min{std::numeric_limits<T>::max()};
    vec _max{-std::numeric_limits<T>::max()};
#else
private:
    vec _min{std::numeric_limits<T>::max()};
    vec _max{-std::numeric_limits<T>::max()};
#endif
};

/**
 * AABB definitions
 */
using Boxf = Box<float>;
using Boxd = Box<double>;

/**
 * Matrix definitions
 */
using Matrix3d = glm::mat<3, 3, double>;
using Matrix3f = glm::mat3;
using Matrix4d = glm::mat<4, 4, double>;
using Matrix4f = glm::mat4;

/**
 * Vector definitions
 */
using Vector2i = glm::vec<2, int32_t>;
using Vector3i = glm::vec<3, int32_t>;

using Vector2ui = glm::vec<2, uint32_t>;
using Vector3ui = glm::vec<3, uint32_t>;

using Vector2f = glm::vec2;
using Vector3f = glm::vec3;
using Vector4f = glm::vec4;

using Vector2d = glm::vec<2, double>;
using Vector3d = glm::vec<3, double>;
using Vector4d = glm::vec<4, double>;

/** A clip plane is defined by a normal and a distance expressed
 * in absolute value of the coordinate system. Values are stored
 * in a Vector4, with the following order: nx, ny, nz and d
 */
using Plane = Vector4d;

/**
 * Quaternion definitions
 */
using Quaternion = glm::quat;
using Quaterniond = glm::tquat<double, glm::highp>; //!< Double quaternion.
} // namespace brayns

namespace std
{
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const brayns::Box<T>& box)
{
    return os << box.getMin() << " - " << box.getMax();
}
} // namespace std
