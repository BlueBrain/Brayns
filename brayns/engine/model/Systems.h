/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <memory>

#include "Components.h"
#include "systemtypes/BoundsSystem.h"
#include "systemtypes/CommitSystem.h"
#include "systemtypes/InitSystem.h"
#include "systemtypes/InspectSystem.h"
#include "systemtypes/ParameterSystem.h"

namespace brayns
{
class Systems
{
public:
    template<typename T, typename... Args>
    void setInitSystem(Args &&...args)
    {
        _init = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void setCommitSystem(Args &&...args)
    {
        _commit = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void setPreRenderSystem(Args &&...args)
    {
        _preRender = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void setPostRenderSystem(Args &&...args)
    {
        _postRender = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void setInspectSystem(Args &&...args)
    {
        _inspect = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void setBoundsSystem(Args &&...args)
    {
        _bounds = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void init(Components &components);
    CommitResult commit(Components &components);
    void preRender(const ParametersManager &parameters, Components &components);
    void postRender(const ParametersManager &parameters, Components &components);
    InspectResult inspect(const InspectContext &context, Components &components);
    Bounds computeBounds(const Matrix4f &matrix, Components &components);

private:
    std::unique_ptr<InitSystem> _init;
    std::unique_ptr<CommitSystem> _commit;
    std::unique_ptr<ParameterSystem> _preRender;
    std::unique_ptr<ParameterSystem> _postRender;
    std::unique_ptr<InspectSystem> _inspect;
    std::unique_ptr<BoundsSystem> _bounds;
};
}
