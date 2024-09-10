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

#include "UseCaseManager.h"

#include <api/usecases/AreaCollage.h>
#include <api/usecases/Density.h>
#include <api/usecases/FlatmapAreas.h>
#include <api/usecases/HighlightColumn.h>
#include <api/usecases/LayerDistance.h>
#include <api/usecases/OrientationField.h>
#include <api/usecases/OutlineShell.h>
#include <api/usecases/VectorField.h>

UseCaseManager UseCaseManager::createDefault()
{
    std::vector<std::unique_ptr<IUseCase>> useCases;
    useCases.push_back(std::make_unique<AreaCollage>());
    useCases.push_back(std::make_unique<Density>());
    useCases.push_back(std::make_unique<HighlightColumn>());
    useCases.push_back(std::make_unique<LayerDistance>());
    useCases.push_back(std::make_unique<OrientationField>());
    useCases.push_back(std::make_unique<VectorField>());
    useCases.push_back(std::make_unique<OutlineShell>());
    useCases.push_back(std::make_unique<FlatmapAreas>());
    return UseCaseManager(std::move(useCases));
}

UseCaseManager::UseCaseManager(std::vector<std::unique_ptr<IUseCase>> useCases):
    _useCases(std::move(useCases))
{
}

const std::vector<std::unique_ptr<IUseCase>> &UseCaseManager::getUseCases() const
{
    return _useCases;
}

const IUseCase &UseCaseManager::getUseCase(const std::string &name) const
{
    auto it = std::find_if(
        _useCases.begin(),
        _useCases.end(),
        [&](const std::unique_ptr<IUseCase> &useCase) { return useCase->getName() == name; });

    if (it == _useCases.end())
    {
        throw std::invalid_argument("Unhandled use-case");
    }

    return **it;
}
