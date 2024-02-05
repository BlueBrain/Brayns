/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/common/MathTypes.h>

#include <memory>

namespace brayns
{
/**
 * @brief The LightType enum defines the different types of light
 */
enum class LightType
{
    SPHERE,
    DIRECTIONAL,
    QUAD,
    SPOTLIGHT,
    AMBIENT
};

/**
 * @brief The Light class defines the common base class for all lights
 */
class Light
{
public:
    Light(LightType type, const Vector3d& color, double intensity,
          bool isVisible);
    Light(LightType type);
    virtual ~Light() = default;

    LightType _type;
    Vector3d _color;
    double _intensity;
    bool _isVisible;
};

using LightPtr = std::shared_ptr<Light>;

class DirectionalLight : public Light
{
public:
    /**
     * @brief DirectionalLight Creates a light that shines from a specific
     * direction not from a specific position. This light will behave as though
     * it is infinitely far away and the rays produced from it are all parallel
     * @param direction Light source direction
     * @param angularDiameter Apparent size (angle in degree) of the light
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     * @param isVisible Whether the light can be directly seen
     */
    DirectionalLight(const Vector3d& direction, double angularDiameter,
                     const Vector3d& color, double intensity, bool isVisible);
    DirectionalLight();

    Vector3d _direction;
    double _angularDiameter;
};

class SphereLight : public Light
{
public:
    /**
     * @brief SphereLight Createas a sphere light. The sphere light (or the
     * special case point light) is a positional light emitting uniformly in all
     * directions.
     * @param position Light source position
     * @param radius The size of the sphere light
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     * @param isVisible Whether the light can be directly seen
     */
    SphereLight(const Vector3d& position, double radius, const Vector3d& color,
                double intensity, bool isVisible);
    SphereLight();

    Vector3d _position;
    double _radius;
};

class QuadLight : public Light
{
public:
    /**
     * @brief QuadLight Creates a quad light. The quad light is a planar,
     * procedural area light source emitting uniformly on one side into the
     * half-space. The emission side is determined by the cross product of edge1
     * x edge2.
     * @param position World-space position of one vertex of the quad light
     * @param edge1 Vector to one adjacent vertex
     * @param edge2 Vector to the other adjacent vertex
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     * @param isVisible Whether the light can be directly seen
     */
    QuadLight(const Vector3d& position, const Vector3d& edge1,
              const Vector3d& edge2, const Vector3d& color, double intensity,
              bool isVisible);
    QuadLight();

    Vector3d _position;
    Vector3d _edge1;
    Vector3d _edge2;
};

class SpotLight : public Light
{
public:
    /**
     * @brief SpotLight Creates a spot light. The spotlight is a light
     * emitting into a cone of directions.
     * @param position The center of the spotlight, in world-space
     * @param direction  Main emission direction of the spot
     * @param openingAngle  Full opening angle (in degree) of the spot; outside
     * of this cone is no illumination
     * @param penumbraAngle Size (angle in degree) of the "penumbra", the region
     * between the rim (of the illumination cone) and full intensity of the
     * spot; should be smaller than half of 'openingAngle'
     * @param radius The size of the spotlight, the radius of a disk with normal
     * 'direction'
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     * @param isVisible Whether the light can be directly seen
     */
    SpotLight(const Vector3d& position, const Vector3d& direction,
              const double openingAngle, const double penumbraAngle,
              const double radius, const Vector3d& color, double intensity,
              bool isVisible);
    SpotLight();

    Vector3d _position;
    Vector3d _direction;
    double _openingAngle;
    double _penumbraAngle;
    double _radius;
};

class AmbientLight : public Light
{
public:
    /**
     * @brief AmbientLight The ambient light surrounds the scene and illuminates
     * it from infinity with constant radiance (determined by combining the
     * parameters color and intensity).
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     * @param isVisible Whether the light can be directly seen
     */
    AmbientLight(const Vector3d& color, double intensity, bool isVisible);
    AmbientLight();
};

} // namespace brayns
