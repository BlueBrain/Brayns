/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include <brayns/core/engine/json/adapters/ModelInstanceAdapter.h>
#include <brayns/core/engine/scene/ModelManager.h>
#include <brayns/core/network/entrypoint/Entrypoint.h>

#include <brayns/atlas/api/UseCaseManager.h>

#include <brayns/atlas/network/messages/VisualizeUseCaseMessage.h>

class VisualizeAtlasUseCaseEntrypoint : public brayns::Entrypoint<VisualizeUseCaseMessage, brayns::ModelInstance>
{
public:
    explicit VisualizeAtlasUseCaseEntrypoint(brayns::ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    brayns::ModelManager &_models;
    UseCaseManager _useCases;
};
