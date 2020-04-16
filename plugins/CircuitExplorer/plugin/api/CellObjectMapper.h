/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/BlueBrain/Brayns>
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

#ifndef CELLOBJECTMAPPER_H
#define CELLOBJECTMAPPER_H

#include <brayns/common/types.h>

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <common/types.h>

#include "MorphologyMap.h"

struct SchemeItem
{
  size_ts ids;
  std::unordered_map<size_t, size_t> materialMap;
  std::unordered_map<size_t, std::string> nameMap;
};

struct CircuitSchemeData
{
    SchemeItem targets;
    SchemeItem etypes;
    SchemeItem mtypes;
    SchemeItem layers;
};

struct RemapCircuitResult
{
    int error;
    std::string message;
};

class CellObjectMapper
{
public:
    void setSourceModel(brayns::ModelDescriptorPtr model);
    void setCircuitSchemes(const CircuitSchemeData& data);

    void add(const size_t gid, const MorphologyMap& mm);
    void remove(const size_t gid);

    RemapCircuitResult remapCircuitColors(const CircuitColorScheme scheme,
                                          brayns::Scene& scene);
    void remapMorphologyColors(const MorphologyColorScheme scheme);

    void onCircuitColorFinish(const CircuitColorScheme& scheme,
                              const MorphologyColorScheme& mScheme);

    CircuitSchemeData& getSchemeData();

private:
    size_t _computeMaterialId(const CircuitColorScheme scheme,
                              const size_t index);

    void _applyDefaultColorMap() const;

private:
    brayns::ModelDescriptorPtr _model;

    CircuitSchemeData _data;

    CircuitColorScheme _lastScheme{CircuitColorScheme::none};
    MorphologyColorScheme _lastMorphologyScheme{MorphologyColorScheme::none};

    // Maps GIDs to all the pieces that forms the morphology of the given cell
    std::unordered_map<size_t, MorphologyMap> _cellToRenderableMap;
};

#endif
