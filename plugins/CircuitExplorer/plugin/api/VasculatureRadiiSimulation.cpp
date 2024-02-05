/* Copyright 2015-2024 Blue Brain Project/EPFL
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
    for (size_t i = 0; i < models.size(); ++i)
    {
        const auto& model = models[i];
        // auto model = scene.getModel(constModel->getModelID());
        auto& modelObj = model->getModel();
        auto handler = modelObj.getSimulationHandler();
        if (auto radiiHandler = toRadiiHandler(handler))
        {
            auto currentFrame = radiiHandler->getBoundedFrame(frame);
            auto lastUsed = radiiHandler->getLastUsedFrame();
            if (lastUsed == frame)
                continue;

            const auto& frameData = radiiHandler->getCurrentRadiiFrame();
            auto& geometries = modelObj.getSDFGeometryData().geometries;

#pragma omp parallel for
            for (size_t geomIdx = 0; geomIdx < geometries.size(); ++geomIdx)
            {
                auto& geometry = geometries[geomIdx];
                geometry.r0 = std::max(frameData[geometry.userData], 0.01f);
                geometry.r1 = std::max(frameData[geometry.userData], 0.01f);
            }

            radiiHandler->setLastUsedFrame(currentFrame);
        }
    }
}
