/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/Scene.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/api/CircuitColorManager.h>
#include <plugin/network/messages/AvailableColoringInfoMessage.h>
#include <plugin/network/messages/ColorCircuitMessage.h>

class ColorCircuitByIdEntrypoint : public brayns::Entrypoint<ColorCircuitByIdMessage, brayns::EmptyMessage>
{
public:
    ColorCircuitByIdEntrypoint(brayns::Scene &scene, CircuitColorManager &manager);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    brayns::Scene &_scene;
    CircuitColorManager &_manager;
};

class ColorCircuitBySingleColorEntrypoint
    : public brayns::Entrypoint<ColorCircuitBySingleColorMessage, brayns::EmptyMessage>
{
public:
    ColorCircuitBySingleColorEntrypoint(brayns::Scene &scene, CircuitColorManager &manager);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    brayns::Scene &_scene;
    CircuitColorManager &_manager;
};

class AvailableColorMethodsEntrypoint : public brayns::Entrypoint<ColoredCircuitID, AvailableColorMethodsMessage>
{
public:
    AvailableColorMethodsEntrypoint(brayns::Scene &scene, CircuitColorManager &manager);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    brayns::Scene &_scene;
    CircuitColorManager &_manager;
};

class AvailableColorMethodVariablesEntrypoint
    : public brayns::Entrypoint<RequestCircuitColorMethodVariables, AvailableColorMethodVariablesMessage>
{
public:
    AvailableColorMethodVariablesEntrypoint(brayns::Scene &scene, CircuitColorManager &manager);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    brayns::Scene &_scene;
    CircuitColorManager &_manager;
};

class ColorCircuitByMethodEntrypoint : public brayns::Entrypoint<ColorCircuitByMethodMessage, brayns::EmptyMessage>
{
public:
    ColorCircuitByMethodEntrypoint(brayns::Scene &scene, CircuitColorManager &manager);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    brayns::Scene &_scene;
    CircuitColorManager &_manager;
};
