/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/Scene.h>

#include <brayns/io/LoaderRegistry.h>

#include <brayns/network/adapters/BinaryParamAdapter.h>
#include <brayns/network/adapters/ModelDescriptorAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/upload/ModelUploadManager.h>

namespace brayns
{
class RequestModelUploadEntrypoint : public Entrypoint<BinaryParam, std::vector<ModelDescriptorPtr>>
{
public:
    RequestModelUploadEntrypoint(Scene &scene, const LoaderRegistry &loaders, ModelUploadManager &modelUploads);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
    virtual bool isAsync() const override;
    virtual void onRequest(const Request &request) override;

private:
    Scene &_scene;
    const LoaderRegistry &_loaders;
    ModelUploadManager &_modelUploads;
};
} // namespace brayns
