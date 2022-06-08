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

#include "UseCaseManager.h"

#include <api/usecases/AreaCollage.h>
#include <api/usecases/Density.h>
#include <api/usecases/HighlightColumn.h>
#include <api/usecases/LayerDistance.h>
#include <api/usecases/OrientationField.h>
#include <api/usecases/OutlineShell.h>
#include <api/usecases/SharedCoordinatesAreaBorders.h>

UseCaseManager::UseCaseManager(std::vector<std::unique_ptr<IUseCase>> useCases)
    : _useCases(std::move(useCases))
{
}

UseCaseManager UseCaseManager::defaultUseCases()
{
    std::vector<std::unique_ptr<IUseCase>> useCases;
    useCases.push_back(std::make_unique<AreaCollage>());
    useCases.push_back(std::make_unique<Density>());
    useCases.push_back(std::make_unique<HighlightColumn>());
    useCases.push_back(std::make_unique<LayerDistance>());
    useCases.push_back(std::make_unique<OrientationField>());
    useCases.push_back(std::make_unique<OutlineShell>());
    useCases.push_back(std::make_unique<SharedCoordinatesAreaBorders>());
    return UseCaseManager(std::move(useCases));
}

std::vector<std::string> UseCaseManager::getValidUseCasesForVolume(const AtlasVolume &volume) const
{
    std::vector<std::string> result;
    result.reserve(_useCases.size());

    for (const auto &useCase : _useCases)
    {
        if (useCase->isVolumeValid(volume))
        {
            result.push_back(useCase->getName());
        }
    }
    return result;
}

std::unique_ptr<brayns::Model> UseCaseManager::executeUseCase(
    const std::string &useCaseName,
    const AtlasVolume &volume,
    const brayns::JsonValue &payload) const
{
    auto it = std::find_if(
        _useCases.begin(),
        _useCases.end(),
        [&](const std::unique_ptr<IUseCase> &useCase) { return useCase->getName() == useCaseName; });

    if (it == _useCases.end())
    {
        throw std::runtime_error("Unhandled use-case");
    }

    const auto &useCase = (**it);
    if (!useCase.isVolumeValid(volume))
    {
        throw std::runtime_error("The volume is not valid to execute the use case " + useCaseName);
    }

    return useCase.execute(volume, payload);
}
