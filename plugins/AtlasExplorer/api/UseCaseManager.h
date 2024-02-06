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

#include <brayns/engine/model/Model.h>

#include "Atlas.h"
#include "IUseCase.h"

#include <memory>
#include <vector>

class UseCaseManager
{
public:
    static UseCaseManager createDefault();

public:
    /**
     * @brief Construct a new UseCaseManager object with the given use-cases to handle
     * @param useCases List of use-cases to handle.
     */
    explicit UseCaseManager(std::vector<std::unique_ptr<IUseCase>> useCases);

    /**
     * @brief Return the all the use cases stored in this manager.
     * @return const std::vector<std::unique_ptr<IUseCase>>&
     */
    const std::vector<std::unique_ptr<IUseCase>> &getUseCases() const;

    /**
     * @brief Returns the use-case handler for the given use-case name.
     * @param name Name of the use case
     * @return const IUseCase& Use-case handler object.
     * @throws std::invalid_argument if the name does not correspond to any existing use-case.
     */
    const IUseCase &getUseCase(const std::string &name) const;

private:
    std::vector<std::unique_ptr<IUseCase>> _useCases;
};
