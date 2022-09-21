/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
#include "ProteinData.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>
#include <brayns/utils/FileReader.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringSplitter.h>

#include <cassert>

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
        auto tokens = brayns::StringSplitter::splitTokens(line);
        assert(tokens.size() >= 12);

        Atom result;
        result.id = std::stoi(tokens[PdbFormatColumns::atomId]);
        result.chainId = static_cast<int32_t>(tokens[PdbFormatColumns::chainId][0]) - 64;
        result.residue = std::stoi(tokens[PdbFormatColumns::residue]);
        result.position.x = std::stof(tokens[PdbFormatColumns::x]);
        result.position.y = std::stof(tokens[PdbFormatColumns::y]);
        result.position.z = std::stof(tokens[PdbFormatColumns::z]);
        result.name = tokens[PdbFormatColumns::name].substr(0, 2);

        return result;
    }
};

class RadiusGenerator
{
public:
    static std::vector<float> generateForAtoms(const brayns::ProteinLoaderParameters &params, std::vector<Atom> &atoms)
    {
        auto &atomRadiis = brayns::ProteinData::atomicRadii;
        auto defaultRadius = brayns::ProteinData::defaultRadius;

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
    inline static constexpr float nanoToMicrons = 0.001f;
    inline static constexpr float angstromsToMicrons = 0.0001f;
    inline static constexpr float positionMultiplier = 10.f;

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
    static std::vector<uint8_t> indexAtoms(
        const brayns::ProteinLoaderParameters &params,
        const std::vector<Atom> &atoms)
    {
        auto &colorMap = brayns::ProteinData::colorIndices;
        auto &colors = brayns::ProteinData::colors;

        auto result = std::vector<uint8_t>();
        result.reserve(atoms.size());

        for (auto &atom : atoms)
        {
            switch (params.color_scheme)
            {
            case brayns::ProteinLoaderColorScheme::ProteinChains:
                result.push_back(static_cast<uint8_t>(atom.chainId));
                break;
            case brayns::ProteinLoaderColorScheme::ProteinResidues:
                result.push_back(static_cast<uint8_t>(atom.residue));
                break;
            case brayns::ProteinLoaderColorScheme::ById:
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
}

namespace brayns
{
std::vector<std::unique_ptr<Model>> ProteinLoader::importFromFile(
    const std::string &path,
    const LoaderProgress &callback,
    const ProteinLoaderParameters &parameters) const
{
    (void)callback;

    auto atoms = PdbReader::readFile(path);
    auto radii = RadiusGenerator::generateForAtoms(parameters, atoms);
    auto spheres = SphereGenerator::generate(atoms, radii);
    auto colorIndices = ColormapIndexer::indexAtoms(parameters, atoms);
    auto &colors = ProteinData::colors;

    auto model = std::make_unique<Model>();

    auto &components = model->getComponents();
    auto &geometries = components.add<Geometries>();
    auto &geometry = geometries.elements.emplace_back(std::move(spheres));
    auto &views = components.add<GeometryViews>();
    auto &view = views.elements.emplace_back(geometry);
    view.setColorMap(ospray::cpp::CopiedData(colorIndices), ospray::cpp::CopiedData(colors));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<GenericBoundsSystem<Geometries>>();
    systems.setInitSystem<GeometryInitSystem>();
    systems.setCommitSystem<GeometryCommitSystem>();

    std::vector<std::unique_ptr<Model>> result;
    result.push_back(std::move(model));
    return result;
}

std::string ProteinLoader::getName() const
{
    return "protein";
}

std::vector<std::string> ProteinLoader::getSupportedExtensions() const
{
    return {"pdb", "pdb1"};
}

std::vector<std::unique_ptr<Model>> ProteinLoader::importFromBlob(
    const Blob &blob,
    const LoaderProgress &callback,
    const ProteinLoaderParameters &parameters) const
{
    (void)blob;
    (void)callback;
    (void)parameters;

    throw std::runtime_error("Loading from blob not supported");
}
} // namespace brayns
