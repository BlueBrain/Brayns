/* Copyright (c) 2019, EPFL/Blue Brain Project
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

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/types.h>

namespace brayns
{
class LightManager : public BaseObject
{
public:
    /**
     * @brief addLight Attaches a light source to the scene
     * @param light Object representing the light source
     */
    void addLight(LightPtr light);

    /**
     * @brief removeLight Removes a light source from the scene for a given id
     * @param id ID of Light source to be removed
     */
    void removeLight(const size_t id);

    /**
     * @brief removeLight Removes a light source from the scene for a given
     * light
     * @param light Light source to be removed
     */
    void removeLight(LightPtr light);

    /**
     * @brief getLight Gets a light source from the scene for a given id.
     * Note: If changing the light then call markModified to propagate the
     * changes.
     * @param id Id of the light
     * @return Pointer to the light, nullptr if not found
     */
    LightPtr getLight(const size_t id);

    /**
     * @brief getLights Gets the lights
     * @return Immutable list of all light sources
     */
    const std::vector<LightPtr>& getLights() const;

    /**
     * @brief clearLights Removes all light sources
     */
    void clearLights();

    /**
     * @brief addDirectionalLight Creates a light that shines from a specific
     * direction not from a specific position. This light will behave as though
     * it is infinitely far away and the rays produced from it are all parallel
     * @param direction Light source direction
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     */
    LightPtr addDirectionalLight(const Vector3f& direction,
                                 const Vector3f& color, float intensity);

    /**
     * @brief addSphereLight Createas a spehre light. The sphere light (or the
     * special case point light) is a light emitting uniformly in all
     * directions.
     * @param position Light source position
     * @param radius The size of the sphere light
     * @param color Light source RGB color
     * @param intensity Amount of light emitted
     */
    LightPtr addSphereLight(const Vector3f& position, float radius,
                            const Vector3f& color, float intensity);

    /**
     * @brief addQuadLight Creates a quad light. The quad light is a planar,
     * procedural area light source emitting uniformly on one side into the
     * half-space.
     * @param position World-space position of one vertex of the quad light
     * @param edge1 Vector to one adjacent vertex
     * @param edge2 Vector to the other adjacent vertex
     * @param color Light source RGB color
     * @param radius The size of the sphere light
     */
    LightPtr addQuadLight(const Vector3f& position, const Vector3f& edge1,
                          const Vector3f& edge2, const Vector3f& color,
                          float intensity);

    /**
     * @brief addSpotLight Creates a spot light. The spotlight is a light
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
     * @param radius The size of the sphere light
     */
    LightPtr addSpotLight(const Vector3f& position, const Vector3f& direction,
                          const float openingAngle, const float penumbraAngle,
                          const float radius, const Vector3f& color,
                          float intensity);

private:
    std::vector<LightPtr> _lights;
    size_t _IDctr{0};
};
} // namespace brayns
