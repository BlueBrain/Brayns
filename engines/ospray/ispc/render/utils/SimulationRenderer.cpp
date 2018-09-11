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
#include <brayns/common/log.h>

namespace brayns
{
void SimulationRenderer::commit()
{
    AbstractRenderer::commit();

    _simulationData = getParamData("simulationData");
    _transferFunctionDiffuseData = getParamData("transferFunctionDiffuseData");
    _transferFunctionEmissionData =
        getParamData("transferFunctionEmissionData");
    _transferFunctionMinValue = getParam1f("transferFunctionMinValue", 0.f);
    _transferFunctionRange = getParam1f("transferFunctionRange", 0.f);
    _alphaCorrection = getParam1f("alphaCorrection", 0.5f);

    const auto transferFunctionDiffuseSize =
        _transferFunctionDiffuseData ? _transferFunctionDiffuseData->size() : 0;
    const auto transferFunctionEmissionSize =
        _transferFunctionEmissionData ? _transferFunctionEmissionData->size()
                                      : 0;

    if (transferFunctionDiffuseSize != transferFunctionEmissionSize)
        BRAYNS_ERROR << "Transfer function diffuse/emission size not the same: "
                     << "'" << transferFunctionDiffuseSize << "' vs '"
                     << transferFunctionEmissionSize << "'" << std::endl;

    _transferFunctionSize =
        std::min(transferFunctionDiffuseSize, transferFunctionEmissionSize);

    _simulationDataSize = _simulationData ? _simulationData->size() : 0;
}

} // ::brayns
