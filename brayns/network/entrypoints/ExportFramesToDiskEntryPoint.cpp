/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *                     nadir.romanguerrero@epfl.ch
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

#include "ExportFramesToDiskEntryPoint.h"

namespace brayns
{
ExportFramesToDiskEntrypoint::ExportFramesToDiskEntrypoint(std::shared_ptr<FrameExporter> &exporter)
    : _exporter(exporter)
{
}

std::string ExportFramesToDiskEntrypoint::getName() const
{
    return "export-frames-to-disk";
}

std::string ExportFramesToDiskEntrypoint::getDescription() const
{
    return "Export a set of frames from a simulation as image files";
}

void ExportFramesToDiskEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    try
    {
        _exporter->startNewExport(std::move(params));
    }
    catch (const FrameExportParameterException &fpe)
    {
        throw EntrypointException(1, fpe.what());
    }
    catch (const FrameExportInProgressException &)
    {
        throw EntrypointException(2, "Frame export already in progress");
    }

    getApi().getEngine().triggerRender();
    request.reply(EmptyMessage());
}

void ExportFramesToDiskEntrypoint::onPreRender()
{
    auto &camera = getApi().getCamera();
    auto &renderer = getApi().getRenderer();
    auto &frameBuffer = getApi().getEngine().getFrameBuffer();
    auto &paramManager = getApi().getParametersManager();

    _exporter->preRender(camera, renderer, frameBuffer, paramManager);
}

void ExportFramesToDiskEntrypoint::onPostRender()
{
    auto &frameBuffer = getApi().getEngine().getFrameBuffer();
    _exporter->postRender(frameBuffer);
}
} // namespace brayns
