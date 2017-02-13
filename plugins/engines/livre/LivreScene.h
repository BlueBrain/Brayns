/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/common/scene/Scene.h>

#include <livre/eq/types.h>

namespace brayns
{

class LivreScene : public Scene
{
public:
    LivreScene( const Renderers& renderer, ParametersManager& parametersManager,
                livre::Engine& livre );

    /** Update transfer function Livre RenderSettings. */
    void commitTransferFunctionData() final;

    /** Update Livre's VolumeRendererParameters, e.g. samples per ray. */
    void commitVolumeData() final;

    /** Unsupported by Livre. */
    void commit() final;

    /** Unsupported by Livre. */
    void buildGeometry() final;

    /** Unsupported by Livre. */
    uint64_t serializeGeometry() final;

    /** Unsupported by Livre. */
    void commitLights() final;

    /** Unsupported by Livre. */
    void commitMaterials( const bool updateOnly = false ) final;

    /** Unsupported by Livre. */
    void commitSimulationData() final;

    /** Unsupported by Livre. */
    void saveSceneToCacheFile() final;

    /** @copydoc Scene::isVolumeSupported */
    bool isVolumeSupported( const std::string& volumeFile ) const final;

private:
    livre::Engine& _livre;
};

}
