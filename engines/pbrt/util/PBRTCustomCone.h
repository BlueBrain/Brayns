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

#pragma once

#include <pbrt/core/shape.h>

/**
 * This class implements the same functionality as PBRT's cone (copied),
 * but with a non physically accurate sampling method to allow
 * the shape to become a light source, used in Brayns for simulation.
 */

namespace brayns
{
class CustomCone : public pbrt::Shape
{
public:
    // Cone Public Methods
    CustomCone(const pbrt::Transform* o2w, const pbrt::Transform* w2o,
               bool reverseOrientation, pbrt::Float height, pbrt::Float radius,
               pbrt::Float phiMax);

    pbrt::Bounds3f ObjectBound() const;

    bool Intersect(const pbrt::Ray& ray, pbrt::Float* tHit,
                   pbrt::SurfaceInteraction* isect,
                   bool testAlphaTexture) const;

    bool IntersectP(const pbrt::Ray& ray, bool testAlphaTexture) const;

    pbrt::Float Area() const;

    pbrt::Interaction Sample(const pbrt::Point2f& u, pbrt::Float* pdf) const;

protected:
    // Cone Private Data
    const pbrt::Float radius, height, phiMax;
};

std::shared_ptr<CustomCone> CreateCustomConeShape(const pbrt::Transform* o2w,
                                                  const pbrt::Transform* w2o,
                                                  bool reverseOrientation,
                                                  const pbrt::ParamSet& params);

} // namespace brayns
