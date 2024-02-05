/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/engine/FrameExporter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/GetExportFramesProgressMessage.h>

namespace brayns
{
class GetExportFramesProgressEntrypoint
    : public Entrypoint<EmptyMessage, GetExportFramesProgressMessage>
{
public:
    virtual std::string getName() const override
    {
        return "get-export-frames-progress";
    }

    virtual std::string getDescription() const override
    {
        return "Get the progress of the last issued frame export";
    }

    virtual void onRequest(const Request& request) override
    {
        double progress{};
        try
        {
            auto& engine = getApi().getEngine();
            auto& frameExporter = engine.getFrameExporter();
            progress = frameExporter.getExportProgress();
        }
        catch (const FrameExportNotRunningException&)
        {
            throw EntrypointException(1,
                                      "There is no frame export in progress");
        }
        catch (const std::runtime_error& e)
        {
            throw EntrypointException(2, e.what());
        }

        request.reply({progress});
    }
};
} // namespace brayns
