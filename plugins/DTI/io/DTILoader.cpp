/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#include "DTILoader.h"
#include "../log.h"
#include "Utils.h"

#include <brayns/engineapi/Material.h>
#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Scene.h>

#include <fstream>

namespace
{
/** Name */
const std::string LOADER_NAME = "DTI loader";

/** Supported extensions */
const std::string SUPPORTED_EXTENTION_DTI = "dti";

template <>
inline std::vector<std::pair<std::string, ColorScheme>> enumerateMap()
{
    return {{"None", ColorScheme::none},
            {"By id", ColorScheme::by_id},
            {"By normal", ColorScheme::by_normal}};
}

struct GidRow
{
    uint64_t gid;
    uint64_t row;
};

std::istream& operator>>(std::istream& in, GidRow& gr)
{
    return in >> gr.gid >> gr.row;
}

/** Properties */
const brayns::Property PROP_RADIUS = {
    "radius", 1., 0.01, 100., {"Streamline radius"}};
const brayns::Property PROP_OPACITY = {
    "opacity", 1., 0., 1., {"Streamline opacity"}};
const brayns::Property PROP_COLOR_SCHEME = {"colorscheme",
                                            enumToString(ColorScheme::none),
                                            enumerateNames<ColorScheme>(),
                                            {"Color scheme"}};

} // namespace

namespace dti
{
DTILoader::DTILoader(brayns::Scene& scene, brayns::PropertyMap&& loaderParams)
    : Loader(scene)
    , _defaults(loaderParams)
{
    PLUGIN_INFO << "Registering " << LOADER_NAME << std::endl;
}

std::string DTILoader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> DTILoader::getSupportedExtensions() const
{
    return {SUPPORTED_EXTENTION_DTI};
}

bool DTILoader::isSupported(const std::string& /*filename*/,
                            const std::string& extension) const
{
    const std::set<std::string> types = {SUPPORTED_EXTENTION_DTI};
    return types.find(extension) != types.end();
}

DTIConfiguration DTILoader::_readConfiguration(
    const boost::property_tree::ptree& pt) const
{
    DTIConfiguration configuration;
    configuration.streamlines = pt.get<std::string>("streamlines");
    configuration.gid_to_streamline =
        pt.get<std::string>("gids_to_streamline_row");
    return configuration;
}

brayns::ModelDescriptorPtr DTILoader::importFromBlob(
    brayns::Blob&& /*blob*/, const brayns::LoaderProgress& /*callback*/,
    const brayns::PropertyMap& /*properties*/) const
{
    throw std::runtime_error("Loading DTI from blob is not supported");
}

Colors DTILoader::getColorsFromPoints(const brayns::Vector3fs& points,
                                      const float opacity,
                                      const ColorScheme colorScheme)
{
    Colors colors;
    switch (colorScheme)
    {
    case ColorScheme::by_normal:
        colors.push_back({0.f, 0.f, 0.f, opacity});
        for (uint64_t i = 0; i < points.size() - 1; ++i)
        {
            const auto& p1 = points[i];
            const auto& p2 = points[i + 1];
            const auto dir = normalize(p2 - p1);
            const brayns::Vector3f n = {0.5f + dir.x * 0.5f,
                                        0.5f + dir.y * 0.5f,
                                        0.5f + dir.z * 0.5f};
            colors.push_back({n.x, n.y, n.z, opacity});
        }
        break;
    case ColorScheme::by_id:
        colors.resize(points.size(),
                      {rand() % 100 / 100.f, rand() % 100 / 100.f,
                       rand() % 100 / 100.f, opacity});
        break;
    default:
        colors.resize(points.size(), {1.f, 1.f, 1.f, opacity});
        break;
    }
    return colors;
}

brayns::ModelDescriptorPtr DTILoader::importFromFile(
    const std::string& filename, const brayns::LoaderProgress& callback,
    const brayns::PropertyMap& properties) const
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(filename, pt);
    const auto config = _readConfiguration(pt);

    brayns::PropertyMap props = _defaults;
    props.merge(properties);

    // Read loading properties
    const auto radius = props.getProperty<double>(PROP_RADIUS.name);
    const auto opacity = props.getProperty<double>(PROP_OPACITY.name);
    const auto colorScheme = stringToEnum<ColorScheme>(
        properties.getProperty<std::string>(PROP_COLOR_SCHEME.name));

    // Check files
    std::ifstream gidRowfile(config.gid_to_streamline, std::ios::in);
    if (!gidRowfile.good())
        PLUGIN_THROW(std::runtime_error("Could not open gid/row mapping file " +
                                        config.gid_to_streamline));

    std::ifstream streamlinesFile(config.streamlines, std::ios::in);
    if (!streamlinesFile.good())
        PLUGIN_THROW(std::runtime_error("Could not open streamlines file " +
                                        config.streamlines));

    // Load positions
    callback.updateProgress("Loading positions ...", 0.f);

    // Load mapping between GIDs and Rows
    callback.updateProgress("Loading mapping ...", 0.2f);
    std::vector<GidRow> gidRows(std::istream_iterator<GidRow>(gidRowfile), {});
    gidRowfile.close();

    // Rows to load
    std::set<u_int64_t> rowsToLoad;
    for (const auto& gidRow : gidRows)
        rowsToLoad.insert(gidRow.row);

    // Load points
    using Points = std::vector<brayns::Vector3f>;
    std::map<uint64_t, Points> streamlines;
    uint64_t count = 0;
    callback.updateProgress("Loading streamlines ...", 0.4f);
    while (streamlinesFile.good())
    {
        std::string line;
        std::getline(streamlinesFile, line);

        if (rowsToLoad.find(count) != rowsToLoad.end())
        {
            std::istringstream in(line);
            uint64_t nbPoints;
            in >> nbPoints;
            Points points;
            for (uint64_t i = 0; i < nbPoints; ++i)
            {
                brayns::Vector3f point;
                in >> point.x >> point.y >> point.z;
                points.push_back(point);
            }
            streamlines[count] = points;
        }
        ++count;
    }
    streamlinesFile.close();

    // Create model
    auto model = _scene.createModel();
    const auto nbStreamlines = gidRows.size();
    count = 0;
    uint64_t i = 0;
    std::set<u_int16_t> streamlineAdded;
    for (const auto& gidRow : gidRows)
    {
        const auto it = streamlines.find(gidRow.row);
        if (it != streamlines.end() &&
            streamlineAdded.find(gidRow.row) == streamlineAdded.end())
        {
            streamlineAdded.insert(gidRow.row);
            callback.updateProgress("Creating " + std::to_string(count) +
                                        " streamlines ...",
                                    0.6f +
                                        0.2f * float(i) / float(nbStreamlines));

            const auto& points = (*it).second;
            const auto nbPoints = points.size();
            const auto colors =
                getColorsFromPoints(points, opacity, colorScheme);

            std::vector<float> radii;
            radii.resize(nbPoints, radius);

            brayns::Streamline streamline(points, colors, radii);
            model->createMaterial(gidRow.gid, std::to_string(gidRow.gid));
            model->addStreamline(gidRow.gid, streamline);
            ++count;
        }
        ++i;
    }

    callback.updateProgress("Committing " + std::to_string(count) +
                                " streamlines ...",
                            0.8f);
    brayns::ModelMetadata metadata = {
        {"Number of streamlines", std::to_string(count)}};
    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "DTI",
                                                  metadata);
    callback.updateProgress("Done", 1.f);
    return modelDescriptor;
}

brayns::PropertyMap DTILoader::getProperties() const
{
    return _defaults;
}

brayns::PropertyMap DTILoader::getCLIProperties()
{
    brayns::PropertyMap pm("DTI");
    pm.setProperty(PROP_RADIUS);
    pm.setProperty(PROP_OPACITY);
    pm.setProperty(PROP_COLOR_SCHEME);
    return pm;
}
} // namespace dti
