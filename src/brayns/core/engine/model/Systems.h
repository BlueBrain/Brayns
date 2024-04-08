/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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
#include "systemtypes/ColorSystem.h"
#include "systemtypes/DataSystem.h"
#include "systemtypes/InspectSystem.h"
#include "systemtypes/UpdateSystem.h"

namespace brayns
{
class Systems
{
public:
    template<typename T, typename... Args>
    void setDataSystem(Args &&...args)
    {
        _data = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void setUpdateSystem(Args &&...args)
    {
        _update = std::make_unique<T>(std::forward<Args>(args)...);
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

    template<typename T, typename... Args>
    void setColorSystem(Args &&...args)
    {
        _color = std::make_unique<T>(std::forward<Args>(args)...);
    }

private:
    friend class SystemsView;
    friend class Model;

    std::unique_ptr<DataSystem> _data;
    std::unique_ptr<UpdateSystem> _update;
    std::unique_ptr<InspectSystem> _inspect;
    std::unique_ptr<BoundsSystem> _bounds;
    std::unique_ptr<ColorSystem> _color;
};
}
