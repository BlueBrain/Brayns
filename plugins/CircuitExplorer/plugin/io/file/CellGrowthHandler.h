/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#ifndef CELLGROWTHHANDLER_H
#define CELLGROWTHHANDLER_H

#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <brayns/api.h>
#include <brayns/common/types.h>

/**
 * @brief The CellGrowthHandler class handles distance to the soma
 */
class CellGrowthHandler : public brayns::AbstractSimulationHandler
{
public:
    /**
     * @brief Default constructor
     */
    CellGrowthHandler(const uint32_t nbFrames);
    CellGrowthHandler(const CellGrowthHandler& rhs);
    ~CellGrowthHandler();

    void* getFrameData(const uint32_t) final;

    bool isReady() const final { return true; }

    brayns::AbstractSimulationHandlerPtr clone() const final;
};
typedef std::shared_ptr<CellGrowthHandler> CellGrowthHandlerPtr;

#endif // CELLGROWTHHANDLER_H
