/*
 * The MIT License (MIT)
 *
 * Copyright © 2021 Nadir Roman Guerrero
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/Plane.h>

namespace brayns
{
std::string_view GeometryOSPRayID<Plane>::get()
{
    return "plane";
}

void GeometryBoundsUpdater<Plane>::update(const Plane &p, const Matrix4f &t, Bounds &b)
{
    // NOOP
    // Planes are infinite. They can be limited, but on Brayns we only use them for clipping
    // https://github.com/ospray/ospray#planes
    (void)p;
    (void)t;
    (void)b;
}

void GeometryCommitter<Plane>::commit(OSPGeometry handle, const std::vector<Plane> &geometry)
{
    auto buffer = DataHandler::shareBuffer(geometry, OSPDataType::OSP_VEC4F);
    ospSetParam(handle, "plane.coefficients", OSP_DATA, &buffer.handle);
}
}
