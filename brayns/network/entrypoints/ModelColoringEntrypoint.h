/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
#include <brayns/network/messages/ColoringMessage.h>
#include <brayns/network/messages/GetModelMessage.h>

#include <brayns/engine/scene/ModelManager.h>

namespace brayns
{
class GetColorMethodsEntrypoint : public Entrypoint<GetModelMessage, std::vector<std::string>>
{
public:
    GetColorMethodsEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    ModelManager &_models;
};

class GetColorValuesEntrypoint : public Entrypoint<ColorMethodValuesMessage, std::vector<std::string>>
{
public:
    GetColorValuesEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    ModelManager &_models;
};

class ColorModelEntrypoint : public Entrypoint<ColorModelMessage, EmptyMessage>
{
public:
    ColorModelEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    ModelManager &_models;
};
}
