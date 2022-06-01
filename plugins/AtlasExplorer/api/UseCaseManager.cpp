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

#include <api/usecases/Density.h>
#include <api/usecases/OrientationField.h>
#include <api/usecases/OutlineShell.h>

namespace
{
class UseCaseRegisterer
{
public:
    template<typename T>
    static void registerUseCase(std::vector<UseCaseManager::UseCaseEntry> &dst)
    {
        auto &entry = dst.emplace_back();
        entry.useCase = T::Type;
        entry.handler = std::make_unique<T>();
    }
};
}

UseCaseManager::UseCaseManager()
{
    UseCaseRegisterer::registerUseCase<Density>(_useCases);
    UseCaseRegisterer::registerUseCase<OrientationField>(_useCases);
    UseCaseRegisterer::registerUseCase<OutlineShell>(_useCases);
}

std::vector<VisualizationUseCase> UseCaseManager::getValidUseCasesForVolume(const AtlasVolume &volume) const
{
    std::vector<VisualizationUseCase> result;
    result.reserve(_useCases.size());

    for (const auto &entry : _useCases)
    {
        const auto &handler = entry.handler;
        if (handler->isVolumeValid(volume))
        {
            result.push_back(entry.useCase);
        }
    }
    return result;
}

void UseCaseManager::executeUseCase(
    VisualizationUseCase useCase,
    const AtlasVolume &volume,
    const brayns::JsonValue &payload,
    brayns::Model &model) const
{
    auto it = std::find_if(
        _useCases.begin(),
        _useCases.end(),
        [useCase](const UseCaseEntry &entry) { return useCase == entry.useCase; });

    if (it == _useCases.end())
    {
        throw std::runtime_error("Unhandled use-case");
    }

    const auto &handler = it->handler;
    handler->execute(volume, payload, model);
}
