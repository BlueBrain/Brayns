/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/adapters/SnapshotAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/entrypoint/EntrypointTask.h>

#include <brayns/tasks/SnapshotTask.h>

namespace brayns
{
class SnapshotTask
    : public EntrypointTask<SnapshotParams, ImageGenerator::ImageBase64>
{
public:
    SnapshotTask(Engine& engine, SnapshotParams params,
                 ImageGenerator& imageGenerator)
        : _functor(engine, std::move(params), imageGenerator)
    {
        _functor.setProgressFunc(
            [this](std::string operation, float, float amount)
            { progress(operation, amount); });
    }

    virtual void run() override { _image = _functor(); }

    virtual void onComplete() override { reply(_image); }

private:
    ImageGenerator::ImageBase64 _image;
    SnapshotFunctor _functor;
};

class SnapshotEntrypoint
    : public Entrypoint<SnapshotParams, ImageGenerator::ImageBase64>
{
public:
    virtual std::string getName() const override { return "snapshot"; }

    virtual std::string getDescription() const override
    {
        return "Take a snapshot with given parameters";
    }

    virtual bool isAsync() const override { return true; }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& engine = getApi().getEngine();
        auto& generator = getImageGenerator();
        auto task = std::make_shared<SnapshotTask>(engine, std::move(params),
                                                   generator);
        launchTask(task, request);
    }
};
} // namespace brayns