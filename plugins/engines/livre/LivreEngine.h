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

#include <brayns/common/engine/Engine.h>

#include <livre/eq/types.h>

namespace brayns
{
class LivreEngine : public Engine
{
public:
    LivreEngine(int argc, char** argv, ParametersManager& parametersManager);

    ~LivreEngine();

    /** @return "livre" */
    std::string name() const final;

    /** @copydoc Engine::commit */
    void commit() final;

    /** @copydoc Engine::render */
    void render() final;

    /**
     * Update the histogram in VolumeHandler after Livre has updated it during
     * the rendering.
     */
    void postRender() final;

    /** @copydoc Engine::getMinimumFrameSize */
    Vector2ui getMinimumFrameSize() const final;

private:
    std::unique_ptr<livre::Engine> _livre;
};
}
