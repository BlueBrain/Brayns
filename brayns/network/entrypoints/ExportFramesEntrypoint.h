/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/messages/ExportFramesMessage.h>

namespace brayns
{
class ExportFramesTask : public EntrypointTask<ExportFramesParams, ExportFramesResult>
{
public:
    ExportFramesTask(Engine &engine, ExportFramesParams &&params);

    void run() final;

    void onComplete() final;

private:
    Engine &_engine;
    ExportFramesParams _params;
    ExportFramesResult _result{0, ""};
};

class ExportFramesEntrypoint : public Entrypoint<ExportFramesParams, ExportFramesResult>
{
public:
    std::string getName() const final;

    std::string getDescription() const final;

    bool isAsync() const final;

    void onRequest(const Request &request) final;
};
} // namespace brayns
