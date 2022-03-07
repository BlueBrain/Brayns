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

#include <brayns/engine/Volume.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/defaultcomponents/TransferFunctionRendererComponent.h>

#include <ospray/ospray.h>

namespace brayns
{
/**
 * @brief Adds a renderable volume to the model
 */
template<typename T>
class VolumeRendererComponent : public Component
{
public:
    VolumeRendererComponent()
    {
        _initializeHandle();
    }

    VolumeRendererComponent(const T& volume)
    {
        _initializeHandle();
        _volume.add(volume);
    }

    ~VolumeRendererComponent()
    {
        if(_model)
            ospRelease(_model);
    }

    virtual uint64_t getSizeInBytes() const noexcept override
    {
        return sizeof(VolumeRendererComponent<T>);
    }

    virtual Bounds computeBounds(const Matrix4f& transform) const noexcept override
    {
        return _volume.computeBounds(transform);
    }

    virtual void onStart() override
    {
        Model& model = getModel();
        auto& group = model.getGroup();
        group.addVolumetricModel(_model);

        try
        {
            auto& tfrComponent = model.getComponent<TransferFunctionRendererComponent>();
        }
        catch (...)
        {
            model.addComponent<TransferFunctionRendererComponent>();
        }
    }

    virtual void onCommit() override
    {
        Model& model = getModel();

        bool needsCommit = false;

        needsCommit = needsCommit || _commitVolume();
        needsCommit = needsCommit || _commitTransferFunction();

        if(needsCommit)
        {
            ospCommit(_model);
        }
    }

    virtual void onDestroyed() override
    {
        auto& model = getModel();
        auto& group = model.getGroup();
        group.removeVolumetricModel(_model);
        ospRelease(_model);
        _model = nullptr;
    }

protected:
    OSPVolumetricModel handle() const noexcept
    {
        return _model;
    }

private:
    void _initializeHandle()
    {
        _model = ospNewVolumetricModel();
    }

    bool _commitVolume()
    {
        if(_volume.isModified())
        {
            _volume.doCommit();
            auto volumeHandle = _volume.handle();
            ospSetParam(_model, "volume", OSPDataType::OSP_VOLUME, &volumeHandle);
            return true;
        }

        return false;
    }

    bool _commitTransferFunction()
    {
        Model& model = getModel();
        auto &tfComponent = model.getComponent<TransferFunctionRendererComponent>();

        if(tfComponent.needsCommit())
        {
            tfComponent.manualCommit();
            auto tfHandle = tfComponent.handle();
            ospSetParam(_model, "transferFunction", OSPDataType::OSP_TRANSFER_FUNCTION, &tfHandle);
            return true;
        }

        return false;
    }

private:
    OSPVolumetricModel _model {nullptr};
    Volume<T> _volume;
};
}
