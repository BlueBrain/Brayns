/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
ExportFramesToDiskEntrypoint::ExportFramesToDiskEntrypoint(
    ParametersManager &parameters,
    Engine &engine,
    FrameExporter &exporter)
    : _parameters(parameters)
    , _engine(engine)
    , _exporter(exporter)
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
        _exporter.startNewExport(std::move(params));
    }
    catch (const FrameExportParameterException &fpe)
    {
        throw JsonRpcException(1, fpe.what());
    }
    catch (const FrameExportInProgressException &)
    {
        throw JsonRpcException(2, "Frame export already in progress");
    }

    _engine.triggerRender();
    request.reply(EmptyMessage());
}

void ExportFramesToDiskEntrypoint::onPreRender()
{
    auto &camera = _engine.getCamera();
    auto &renderer = _engine.getRenderer();
    auto &frameBuffer = _engine.getFrameBuffer();

    _exporter.preRender(camera, renderer, frameBuffer, _parameters);
}

void ExportFramesToDiskEntrypoint::onPostRender()
{
    auto &frameBuffer = _engine.getFrameBuffer();
    _exporter.postRender(frameBuffer);
}
} // namespace brayns
