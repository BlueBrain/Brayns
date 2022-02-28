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

#include <brayns/common/TransferFunction.h>
#include <brayns/engine/Model.h>

namespace brayns
{
/**
 * @brief The VolumetricModel class is a candidate base class for volume based models with a transfer function
 */
class VolumetricModel : public Model
{
public:
    /**
     * @brief Initializes the OSPRay handle and the material to a default one
     */
    VolumetricModel();

    virtual ~VolumetricModel();

    /**
     * @brief Returns a constant factor applied to all the volume cells
     */
    float getDensityScale() const noexcept;

    /**
     * @brief Sets a constant value by which all the volume cell values will be multiplied
     */
    void setDensityScale(const float scale) noexcept;

    /**
     * @brief Sets tranfer function used to render the volume
     */
    void setTransferFunction(TransferFunction tf);

    /**
     * @brief Returns the transfer function of this model
     */
    const TransferFunction& getTransferFunction() const noexcept;

    /**
     * @brief Commit implementation
     */
    void commit() final;

    /**
     * @brief Returns the volumetric model OSPRay handle
     */
    OSPVolumetricModel handle() const noexcept;

protected:
    /**
     * @brief Subclasses must implement this to commit their specific volume data, and then
     * set the the "volume" parameter of this VolumetricModel
     */
    virtual void commitVolumeModel() = 0;

private:
    TransferFunction _transferFunction;

    float _densityScale {1.f};

    OSPVolumetricModel _handle {nullptr};
    OSPTransferFunction _tfHandle {nullptr};
};
}
