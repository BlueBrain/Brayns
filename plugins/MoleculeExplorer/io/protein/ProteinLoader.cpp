/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ProteinLoader.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <brayns/utils/FileReader.h>
#include <brayns/utils/Log.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringSplitter.h>

#include <cassert>

#include <ospray/ospray_cpp/ext/rkcommon.h>

#include "ProteinData.h"

namespace
{
/**
 * Structure defining an atom as it is stored in a PDB file
 */
struct Atom
{
    int32_t id;
    int32_t chainId;
    int32_t residue;
    brayns::Vector3f position;
    std::string name;
};

/*
 * Unused, but left for token position reference
struct PdbFormatColumns
{
    inline static constexpr size_t type = 0;
    inline static constexpr size_t atomId = 1;
    inline static constexpr size_t chainId = 4;
    inline static constexpr size_t residue = 5;
    inline static constexpr size_t x = 6;
    inline static constexpr size_t y = 7;
    inline static constexpr size_t z = 8;
    inline static constexpr size_t name = 11;
};
*/

class PdbReader
{
public:
    static std::vector<Atom> readFile(const std::string &path)
    {
        auto content = brayns::FileReader::read(path);
        return _processLines(content);
    }

private:
    static std::vector<Atom> _processLines(std::string_view content)
    {
        auto tempBuffer = _allocateBuffer(content);

        auto line = brayns::StringExtractor::extractLine(content);
        while (!line.empty())
        {
            if (_filterLine(line))
            {
                auto atom = _processLine(line);
                tempBuffer.push_back(std::move(atom));
            }
            brayns::StringExtractor::extract(content, 1);
            line = brayns::StringExtractor::extractLine(content);
        }

        return tempBuffer;
    }

    static bool _filterLine(std::string_view line)
    {
        if (line.find("ATOM") == 0 || line.find("HETATM") == 0)
        {
            return true;
        }
        return false;
    }

    static std::vector<Atom> _allocateBuffer(std::string_view content)
    {
        auto lines = brayns::StringCounter::countLines(content);
        auto buffer = std::vector<Atom>();
        buffer.reserve(lines);
        return buffer;
    }

    static Atom _processLine(std::string_view line)
    {
        auto tokenCount = brayns::StringCounter::countTokens(line);
        if (tokenCount < 12)
        {
            throw std::runtime_error("Invalid number of tokens on line");
        }

        brayns::StringExtractor::extractToken(line);
        auto atomIdToken = brayns::StringExtractor::extractToken(line);
        brayns::StringExtractor::extractToken(line);
        brayns::StringExtractor::extractToken(line);
        auto chainToken = brayns::StringExtractor::extractToken(line);
        auto residueToken = brayns::StringExtractor::extractToken(line);
        auto xToken = brayns::StringExtractor::extractToken(line);
        auto yToken = brayns::StringExtractor::extractToken(line);
        auto zToken = brayns::StringExtractor::extractToken(line);
        brayns::StringExtractor::extractToken(line);
        brayns::StringExtractor::extractToken(line);
        auto nameToken = brayns::StringExtractor::extractToken(line);

        Atom result;
        result.id = std::stoi(std::string(atomIdToken));
        result.chainId = static_cast<int32_t>(chainToken[0]) - 64;
        result.residue = std::stoi(std::string(residueToken));
        result.position.x = std::stof(std::string(xToken));
        result.position.y = std::stof(std::string(yToken));
        result.position.z = std::stof(std::string(zToken));
        result.name = nameToken.substr(0, 2);

        return result;
    }
};

class RadiusGenerator
{
public:
    static std::vector<float> generateForAtoms(const ProteinLoaderParameters &params, std::vector<Atom> &atoms)
    {
        auto &atomRadiis = ProteinData::atomicRadii;
        auto defaultRadius = ProteinData::defaultRadius;

        std::vector<float> result;
        result.reserve(atoms.size());

        for (auto &atom : atoms)
        {
            auto it = std::find(atomRadiis.begin(), atomRadiis.end(), atom.name);
            auto radius = it == atomRadiis.end() ? defaultRadius : it->radius;
            radius *= params.radius_multiplier;
            result.push_back(radius);
        }

        return result;
    }
};

class SphereGenerator
{
public:
    static inline constexpr float nanoToMicrons = 0.001f;
    static inline constexpr float angstromsToMicrons = 0.0001f;
    static inline constexpr float positionMultiplier = 10.f;

    static std::vector<brayns::Sphere> generate(const std::vector<Atom> &atoms, const std::vector<float> &radii)
    {
        std::vector<brayns::Sphere> spheres;
        spheres.reserve(atoms.size());

        for (size_t i = 0; i < atoms.size(); ++i)
        {
            auto position = atoms[i].position * nanoToMicrons * positionMultiplier;
            auto radius = radii[i] * angstromsToMicrons;
            spheres.push_back({position, radius});
        }

        return spheres;
    }
};

class ColormapIndexer
{
public:
    static std::vector<uint8_t> indexAtoms(const ProteinLoaderParameters &params, const std::vector<Atom> &atoms)
    {
        auto &colorMap = ProteinData::colorIndices;
        auto &colors = ProteinData::colors;

        auto result = std::vector<uint8_t>();
        result.reserve(atoms.size());

        for (auto &atom : atoms)
        {
            switch (params.color_scheme)
            {
            case ProteinLoaderColorScheme::ProteinChains:
                result.push_back(static_cast<uint8_t>(atom.chainId));
                break;
            case ProteinLoaderColorScheme::ProteinResidues:
                result.push_back(static_cast<uint8_t>(atom.residue));
                break;
            case ProteinLoaderColorScheme::ById:
                result.push_back(atom.id % colors.size());
                break;
            default:
                auto it = std::find(colorMap.begin(), colorMap.end(), atom.name);
                auto index = it == colorMap.end() ? 0 : it->colorIndex;
                result.push_back(static_cast<uint8_t>(index));
            }
        }
        return result;
    }
};
} // namespace

std::string ProteinLoader::getName() const
{
    return "protein";
}

std::vector<std::string> ProteinLoader::getExtensions() const
{
    return {"pdb", "pdb1"};
}

std::vector<std::shared_ptr<brayns::Model>> ProteinLoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);
    auto &params = request.params;

    brayns::Log::info("[ME] Loading protein file {}.", path);

    auto atoms = PdbReader::readFile(path);
    auto radii = RadiusGenerator::generateForAtoms(params, atoms);
    auto spheres = SphereGenerator::generate(atoms, radii);
    auto colorIndices = ColormapIndexer::indexAtoms(params, atoms);
    auto &colors = ProteinData::colors;

    auto model = std::make_shared<brayns::Model>("protein");

    auto &components = model->getComponents();
    auto &geometries = components.add<brayns::Geometries>();
    auto &geometry = geometries.elements.emplace_back(std::move(spheres));
    auto &views = components.add<brayns::GeometryViews>();
    auto &view = views.elements.emplace_back(geometry);
    view.setColorMap(ospray::cpp::CopiedData(colorIndices), ospray::cpp::CopiedData(colors));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
    systems.setDataSystem<brayns::GeometryDataSystem>();

    std::vector<std::shared_ptr<brayns::Model>> result;
    result.push_back(std::move(model));
    return result;
}
