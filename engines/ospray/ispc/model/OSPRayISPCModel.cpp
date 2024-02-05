/* Copyright 2021-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "OSPRayISPCModel.h"

// ispc exports
#include "OSPRayISPCModel_ispc.h"

namespace brayns
{
OSPRayISPCModel::OSPRayISPCModel()
{
    managedObjectType = OSP_MODEL;
    this->ispcEquivalent = ispc::OSPRayISPCModel_create(this);
}

OSPRayISPCModel::~OSPRayISPCModel() {}

void OSPRayISPCModel::commit()
{
    simEnabled = getParam<bool>("simEnabled", false);
    simOffset = getParam1i("simOffset", 0);

    ispc::OSPRayISPCModel_init(getIE(), simEnabled, simOffset);

    Model::commit();
}

std::string OSPRayISPCModel::toString() const
{
    return std::string("brayns::OSPRayISPCModel");
}

} // namespace brayns
