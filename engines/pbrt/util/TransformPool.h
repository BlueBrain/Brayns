/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#ifndef TRANSFORMPOOL_H
#define TRANSFORMPOOL_H

#include <pbrt/core/memory.h>
#include <pbrt/core/transform.h>

#include <unordered_set>

namespace brayns
{
struct TransformHash
{
    bool operator()(const pbrt::Transform* const t) const
    {
        // From pbrt: src/core/api.cpp TransformCache::Hash
        const char *ptr = reinterpret_cast<const char*>( (&t->GetMatrix()) );
        size_t size = sizeof(pbrt::Matrix4x4);
        uint64_t hash = 14695981039346656037ull;
        while (size > 0) {
            hash ^= static_cast<uint64_t>(*ptr);
            hash *= 1099511628211ull;
            ++ptr;
            --size;
        }
        return hash;
    }
};
struct TransformEqual
{
    bool operator()(const pbrt::Transform* const a, const pbrt::Transform* const b) const
    {
        return *a == *b;
    }
};

class TransformPool
{
public:
    pbrt::Transform* findOrCreate(pbrt::Transform& t);
    void clear();

private:
    std::unordered_set<pbrt::Transform*, TransformHash, TransformEqual> _hasTable;
    pbrt::MemoryArena _pool;
};
}

#endif
