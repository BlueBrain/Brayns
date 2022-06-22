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

#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/components/TransferFunctionRendererComponent.h>
#include <brayns/engine/volume/VolumeObject.h>

namespace brayns
{
/**
 * @brief Adds a renderable volume to the model
 */
template<typename T>
class VolumeRendererComponent final : public Component
{
public:
    VolumeRendererComponent(T volumeData)
        : _object(std::move(volumeData))
    {
    }

    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override
    {
        return _object.computeBounds(transform);
    }

    virtual void onCreate() override
    {
        Model &model = getModel();
        auto &group = model.getGroup();
        group.addVolume(_object);

        model.addComponent<TransferFunctionRendererComponent>();
    }

    bool commit() override
    {
        Model &model = getModel();
        auto &tfComponent = model.getComponent<TransferFunctionRendererComponent>();
        if (tfComponent.manualCommit())
        {
            _object.setTransferFunction(tfComponent.getOsprayObject());
        }

        return _object.commit();
    }

    virtual void onDestroy() override
    {
        auto &model = getModel();
        auto &group = model.getGroup();
        group.removeVolume(_object);
    }

private:
    VolumeObject<T> _object;
};
}
