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

#include <plugin/io/sonataloader/reports/handlers/VasculatureRadiiHandler.h>

std::vector<VasculatureRadiiSimulation::SimulationTracker>
    VasculatureRadiiSimulation::_vasculatureModels;

void VasculatureRadiiSimulation::registerModel(
    brayns::ModelDescriptorPtr& model)
{
    // Must have simulation
    auto handler = model->getModel().getSimulationHandler();
    if (!handler)
        return;

    // And must be a vasculature radii handler
    auto radiiHandler =
        dynamic_cast<sonataloader::VasculatureRadiiHandler*>(handler.get());
    if (radiiHandler == nullptr)
        return;

    const auto maxIndex = radiiHandler->getRadiiFrameSize() - 1;
    for (auto& sdfGeometry : model->getModel().getSDFGeometryData().geometries)
    {
        if (sdfGeometry.userData > maxIndex)
            throw std::runtime_error(
                "VasculatureRadiiSimulation: Vasculature geometry is "
                "mapped beyond frame size");
    }

    _vasculatureModels.push_back(
        {model.get(), std::numeric_limits<uint32_t>::max()});
}

void VasculatureRadiiSimulation::unregisterModel(size_t modelId)
{
    auto it = std::find_if(_vasculatureModels.begin(), _vasculatureModels.end(),
                           [modId = modelId](const SimulationTracker& st) {
                               return st.model->getModelID() == modId;
                           });

    if (it != _vasculatureModels.end())
        _vasculatureModels.erase(it);
}

void VasculatureRadiiSimulation::update()
{
    for (size_t i = 0; i < _vasculatureModels.size(); ++i)
    {
        auto& model = _vasculatureModels[i].model->getModel();
        auto handler = static_cast<sonataloader::VasculatureRadiiHandler*>(
            model.getSimulationHandler().get());

        const auto boundedFrame = handler->getLastBoundedFrame();
        if (boundedFrame != _vasculatureModels[i].lastFrame)
        {
            const auto radii = handler->getCurrentRadiiFrame();

            for (auto& sdfGeometry : model.getSDFGeometryData().geometries)
            {
                sdfGeometry.r0 = radii[sdfGeometry.userData];
                sdfGeometry.r1 = radii[sdfGeometry.userData];
            }

            model.commitGeometry();
            _vasculatureModels[i].lastFrame = boundedFrame;
        }
    }
}
