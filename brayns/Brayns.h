/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef BRAYNS_H
#define BRAYNS_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/common/material/Material.h>

namespace brayns
{

struct RenderInput
{
    Vector2i windowSize;
    Matrix4f modelview;
    Matrix4f projection;

    Vector3f position;
    Vector3f target;
    Vector3f up;
};

struct RenderOutput
{
    uint8_ts colorBuffer;
    floats depthBuffer;
};

const size_t DEFAULT_NB_MATERIALS = 200;

/** Off-screen rendering engine for interactive CPU based ray-tracing
 */
class Brayns
{
public:

    BRAYNS_API Brayns(int argc, const char **argv);
    BRAYNS_API ~Brayns();

    /** Sets the rendering parameters and renders the current frame and
     * returns the render output
     */
    BRAYNS_API void render(
        const RenderInput& renderInput,
        RenderOutput& renderOutput);

    BRAYNS_API void commit();

    BRAYNS_API RenderingParametersPtr getRenderingParameters();

    BRAYNS_API GeometryParametersPtr getGeometryParameters();

    BRAYNS_API ApplicationParametersPtr getApplicationParameters();

    BRAYNS_API void setMaterials(
        MaterialType materialType,
        size_t nbMaterials = DEFAULT_NB_MATERIALS);

    BRAYNS_API void resize(const Vector2i& frameSize);

    BRAYNS_API const ScenePtr getScene() const;

private:

    struct Impl;

    std::unique_ptr<Impl> _impl;

};

}
#endif // BRAYNS
