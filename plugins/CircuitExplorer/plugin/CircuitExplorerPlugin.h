/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include <brayns/pluginapi/ExtensionPlugin.h>

#include <plugin/api/CircuitColorManager.h>

/**
 * @brief The CircuitExplorerPlugin class manages the loading and visualization
 * of the Blue Brain Project micro-circuits, and allows visualisation of voltage
 * simulations
 */
class CircuitExplorerPlugin : public brayns::ExtensionPlugin
{
public:
    CircuitExplorerPlugin();

    void init() final;
    void preRender() final;
    void postRender() final;

private:
    CircuitColorManager _colorManager;
};
