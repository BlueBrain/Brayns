/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "SimulationRenderer.h"
#include "SimulationRenderer_ispc.h"

#include <ospray/SDK/transferFunction/TransferFunction.h>

namespace brayns
{
void SimulationRenderer::commit()
{
    AbstractRenderer::commit();

    _simulationData = getParamData("simulationData");
    _alphaCorrection = getParam1f("alphaCorrection", 0.5f);

    ospray::TransferFunction* transferFunction =
        (ospray::TransferFunction*)getParamObject("transferFunction", nullptr);
    if (transferFunction)
        ispc::SimulationRenderer_setTransferFunction(getIE(),
                                                     transferFunction->getIE());

    _simulationDataSize = _simulationData ? _simulationData->size() : 0;
}

} // namespace brayns
