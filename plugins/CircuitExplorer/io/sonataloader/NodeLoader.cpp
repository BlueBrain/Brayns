/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "NodeLoader.h"

#include "LoaderTable.h"

namespace
{
class NodePopulationLoaders
{
public:
    NodePopulationLoaders()
    {
        _table.registerLoader<AstrocytePopulationLoader>();
        _table.registerLoader<BiophysicalPopulationLoader>();
        _table.registerLoader<PointNeuronPopulationLoader>();
        _table.registerLoader<VasculaturePopulationLoader>();
    }

    const NodePopulationLoader &getNodeLoader(const std::string &name)
    {
        auto nlptr = _table.getLoader(name);
        if (!nlptr)
            throw std::invalid_argument("No node population loader for type " + name);

        return *nlptr;
    }

private:
    PopulationLoaderTable<NodePopulationLoader> _table;
};

class EdgePopulationLoaders
{
public:
    EdgePopulationLoaders()
    {
        _table.registerLoader<ChemicalSynapsePopulationLoader>();
        _table.registerLoader<ElectricalSynapsePopulationLoader>();
        _table.registerLoader<EndFootPopulationLoader>();
        _table.registerLoader<GlialGlialPopulationLoader>();
        _table.registerLoader<SynapseAstrocytePopulationLoader>();
    }

    const EdgePopulationLoader &getEdgeLoader(const std::string &name)
    {
        auto elptr = _table.getLoader(name);
        if (!elptr)
            throw std::invalid_argument("No edge population loader for type " + name);

        return *elptr;
    }

private:
    PopulationLoaderTable<EdgePopulationLoader> _table;
};
}

namespace sonataloader
{
std::vector<CellCompartments>
    NodeLoader::loadNodes(const NodeLoadContext &ctxt, ProgressUpdater &updater, brayns::Model &model)
{
}
}
