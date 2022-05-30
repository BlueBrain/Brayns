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

#include <brayns/common/MathTypes.h>

#include <api/DataMangler.h>
#include <api/IAtlasVolume.h>

class ScalarVolume final : public IAtlasVolume
{
public:
    ScalarVolume(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, std::unique_ptr<IDataMangler> data);

    void handleUseCase(VisualizationUseCase useCase, brayns::Model &model) const override;

    const brayns::Vector3ui &getGridSize() const noexcept;

    const brayns::Vector3f &getGridSpacing() const noexcept;

    const IDataMangler &getData() const noexcept;

private:
    brayns::Vector3ui _gridSize;
    brayns::Vector3f _gridSpacing;
    std::unique_ptr<IDataMangler> _data;
};
