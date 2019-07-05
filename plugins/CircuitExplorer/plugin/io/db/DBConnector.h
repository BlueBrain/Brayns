/* Copyright (c) 2019, EPFL/Blue Brain Project
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

#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <pqxx/pqxx>

#include <brayns/common/Transformation.h>
#include <brayns/common/types.h>

const size_t NB_CONNECTIONS = 19;

using Matrix3f = glm::mat<3, 3, float>;
using Quaternionf = glm::tquat<float, glm::highp>;

using Point = brayns::Vector3f;
using Points = std::vector<Point>;
using Streamlines = std::map<uint64_t, Points>;

struct Cell
{
    brayns::Transformation transformation;
    uint64_t morphologyId;
};

using Cells = std::vector<Cell>;

class DBConnector
{
public:
    DBConnector(const std::string& connectionString);
    ~DBConnector();

    Cells getCells(const std::string& sqlStatement = "");
    std::vector<std::string> getMorphologyPaths(
        const std::string& sqlStatement = "");

    void loadCells(brayns::Scene& scene, const std::string& name,
                   const std::string& sqlCell,
                   const std::string& sqlMorphology);

    void loadCellsAsSDF(brayns::Scene& scene, const std::string& name,
                        const std::string& sqlCell,
                        const std::string& sqlMorphology);

    void loadCellOrientations(brayns::Scene& scene, const std::string& name,
                              const std::string& sqlStatement,
                              const float radius);

    void loadSomas(brayns::Scene& scene, const std::string& name,
                   const std::string& sqlStatement, const float radius);

    void loadSegments(brayns::Scene& scene, const std::string& name,
                      const std::string& sqlStatement, const float radius);

    void loadSynapses(brayns::Scene& scene, const std::string& name,
                      const std::string& sqlStatement, const float radius);

    void loadMeshes(brayns::Scene& scene, const std::string& sqlStatement);

    void importMorphology(brayns::Scene& scene, const uint64_t guid,
                          const std::string& filename);
    void importMorphologyAsSDF(brayns::Scene& scene, const uint64_t guid,
                               const std::string& filename);
    void importVolume(const uint64_t guid, const brayns::Vector3f& dimensions,
                      const brayns::Vector3f& spacing,
                      const std::string& filename);
    void importCompartmentSimulation(const std::string blueConfig,
                                     const std::string reportName);

    std::vector<float> loadSomaSimulation(const uint64_t frame,
                                          const std::string& sqlStatement = "");

    std::map<uint64_t, std::vector<float>> loadSomaSimulation();

private:
    pqxx::connection _connection;
    std::string _connectionString;
};

#endif // DBCONNECTOR_H
