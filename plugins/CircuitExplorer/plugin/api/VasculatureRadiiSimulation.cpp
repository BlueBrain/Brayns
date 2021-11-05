/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "VasculatureRadiiSimulation.h"

#include <brayns/engine/Model.h>

#include <plugin/io/sonataloader/reports/handlers/VasculatureRadiiHandler.h>

namespace
{
sonataloader::VasculatureRadiiHandler* toRadiiHandler(
    brayns::AbstractSimulationHandlerPtr& handler)
{
    return dynamic_cast<sonataloader::VasculatureRadiiHandler*>(handler.get());
}
} // namespace

void VasculatureRadiiSimulation::update(const uint32_t frame,
                                        brayns::Scene& scene)
{
    const auto& models = scene.getModels();

#pragma omp parallel for
    for (size_t i = 0; i < models.size(); ++i)
    {
        const auto& model = models[i];
        auto& modelObj = model->getModel();
        auto handler = modelObj.getSimulationHandler();

        if (auto radiiHandler = toRadiiHandler(handler))
        {
            const auto& frameData = radiiHandler->getCurrentRadiiFrame();
            for (auto& sdfGeometry : modelObj.getSDFGeometryData().geometries)
            {
                sdfGeometry.r0 = frameData[sdfGeometry.userData];
                sdfGeometry.r1 = frameData[sdfGeometry.userData];
            }
            modelObj.commitGeometry();
        }
    }
}
