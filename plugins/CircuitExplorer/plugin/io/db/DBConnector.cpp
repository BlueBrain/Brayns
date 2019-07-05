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

#include "DBConnector.h"
#include <common/log.h>
#include <common/types.h>
#include <io/file/MorphologyLoader.h>

#include <brayns/common/Timer.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/io/MeshLoader.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <fstream>
#include <omp.h>

DBConnector::DBConnector(const std::string& connectionString)
    : _connection(connectionString)
    , _connectionString(connectionString)
{
}

DBConnector::~DBConnector()
{
    _connection.disconnect();
}

Cells DBConnector::getCells(const std::string& sqlStatement)
{
    Cells cells;
    try
    {
        pqxx::read_transaction transaction(_connection);
        const auto sql = sqlStatement.empty()
                             ? "SELECT cp.x, cp.y, cp.z, co.x, co.y, co.z, "
                               "co.w, c.morphology_guid FROM cell AS c, "
                               "cell_position AS cp, cell_orientation AS co "
                               "WHERE c.guid = cp.guid AND c.guid = co.guid"
                             : sqlStatement;

        auto res = transaction.exec(sql);
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            if (c.size() != 8)
            {
                const std::string msg =
                    "Invalid SQL statement. The following selection is "
                    "expected: " +
                    sql;
                PLUGIN_THROW(msg);
            }

            Cell cell;
            const brayns::Vector3f center = {c[0].as<float>(), c[1].as<float>(),
                                             c[2].as<float>()};
            cell.transformation.setScale({1.f, 1.f, 1.f});
            cell.transformation.setTranslation(center);
            //            cell.transformation.setRotationCenter(center);
            cell.transformation.setRotation({c[6].as<float>(), c[3].as<float>(),
                                             c[4].as<float>(),
                                             c[5].as<float>()});
            cell.morphologyId = c[7].as<uint64_t>();
            cells.push_back(cell);
        }
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
    return cells;
}

std::vector<std::string> DBConnector::getMorphologyPaths(
    const std::string& sqlStatement)
{
    std::vector<std::string> paths;
    try
    {
        pqxx::read_transaction transaction(_connection);
        const auto sql =
            sqlStatement.empty() ? "SELECT name FROM morphology" : sqlStatement;
        auto res = transaction.exec(sql);
        for (auto c = res.begin(); c != res.end(); ++c)
            paths.push_back(c[0].as<std::string>());
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
    return paths;
}

void DBConnector::loadCells(brayns::Scene& scene, const std::string& name,
                            const std::string& sqlCell,
                            const std::string& sqlMorphology)
{
    PLUGIN_INFO << "Loading cells from DB..." << std::endl;

    struct CellContent
    {
        uint64_t nbSpheres{0};
        std::vector<brayns::Sphere> spheres{};
        uint64_t nbCylinders{0};
        std::vector<brayns::Cylinder> cylinders{};
        uint64_t nbCones{0};
        std::vector<brayns::Cone> cones{};
    };
    std::map<uint64_t, CellContent> cellContents;

    brayns::Timer chrono0;
    try
    {
        // Read morphology contents
        PLUGIN_DEBUG << sqlMorphology << std::endl;
        pqxx::read_transaction transaction(_connection);
        pqxx::result res = transaction.exec(sqlMorphology);

        brayns::Timer chrono1;
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            CellContent cc;
            const auto guid = c[0].as<uint64_t>();

            for (pqxx::row::size_type column = 1; column < c.size(); ++column)
            {
                const pqxx::binarystring bytea(c.at(column));
                auto indexInBuffer = bytea.data();

                // Load spheres
                uint64_t nbElements;
                memcpy(&nbElements, indexInBuffer, sizeof(uint64_t));
                indexInBuffer += sizeof(uint64_t);
                cc.spheres.resize(cc.nbSpheres + nbElements);
                memcpy(&cc.spheres[cc.nbSpheres], indexInBuffer,
                       sizeof(brayns::Sphere) * nbElements);
                indexInBuffer += sizeof(brayns::Sphere) * nbElements;
                cc.nbSpheres += nbElements;

                // Load cylinders
                memcpy(&nbElements, indexInBuffer, sizeof(uint64_t));
                indexInBuffer += sizeof(uint64_t);
                cc.cylinders.resize(cc.nbCylinders + nbElements);
                memcpy(&cc.cylinders[cc.nbCylinders], indexInBuffer,
                       sizeof(brayns::Cylinder) * nbElements);
                indexInBuffer += sizeof(brayns::Cylinder) * nbElements;
                cc.nbCylinders += nbElements;

                // Load cones
                memcpy(&nbElements, indexInBuffer, sizeof(uint64_t));
                indexInBuffer += sizeof(uint64_t);
                cc.cones.resize(cc.nbCones + nbElements);
                memcpy(&cc.cones[cc.nbCones], indexInBuffer,
                       sizeof(brayns::Cone) * nbElements);
                cc.nbCones += nbElements;
            }

            cellContents[guid] = cc;

            PLUGIN_DEBUG << cc.nbSpheres << " spheres, " << cc.nbCylinders
                         << " cylinders and " << cc.nbCones << " cones"
                         << std::endl;
        }
        const auto dbTime = chrono1.elapsed();
        PLUGIN_TIMER(dbTime, "Loading of " << cellContents.size()
                                           << " unique cell contents");

        // Load morphologies
        res = transaction.exec(sqlCell);

        const auto nbCells = res.size();
        auto model = scene.createModel();

        brayns::Timer chrono2;

        uint64_t materialId = 0;
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const brayns::Vector3f position{c[0].as<float>(), c[1].as<float>(),
                                            c[2].as<float>()};
            const Quaternionf rotation{c[6].as<float>(), c[3].as<float>(),
                                       c[4].as<float>(), c[5].as<float>()};
            const auto morphologyGuid = c[7].as<uint64_t>();

            if (cellContents.find(morphologyGuid) == cellContents.end())
            {
                PLUGIN_ERROR << "Morphology " << morphologyGuid
                             << " contents have not been loaded" << std::endl;
                continue;
            }
            const auto& cc = cellContents[morphologyGuid];

            model->createMaterial(materialId, std::to_string(materialId));

            auto& spheres = model->getSpheres()[materialId];
            spheres.resize(cc.nbSpheres);
            std::copy(cc.spheres.begin(), cc.spheres.end(), spheres.begin());
            for (auto& sphere : spheres)
                sphere.center = position + rotation * sphere.center;

            auto& cylinders = model->getCylinders()[materialId];
            cylinders.resize(cc.nbCylinders);
            std::copy(cc.cylinders.begin(), cc.cylinders.end(),
                      cylinders.begin());
            for (auto& cylinder : cylinders)
            {
                cylinder.center = position + rotation * cylinder.center;
                cylinder.up = position + rotation * cylinder.up;
            }

            auto& cones = model->getCones()[materialId];
            cones.resize(cc.nbCones);
            std::copy(cc.cones.begin(), cc.cones.end(), cones.begin());
            for (auto& cone : cones)
            {
                cone.center = position + rotation * cone.center;
                cone.up = position + rotation * cone.up;
            }

            // Add soma
            model->addSphere(materialId, {position, 1.f});
            ++materialId;
        }
        const auto buildingTime = chrono2.elapsed();
        PLUGIN_TIMER(buildingTime, "Creation and transformation of "
                                       << res.size() << " cells");

        brayns::Timer chrono3;
        brayns::ModelMetadata metadata = {
            {"Number of cells", std::to_string(nbCells)},
            {"SQL for cells", sqlCell},
            {"SQL for morphologies", sqlMorphology},
            {"DB time", std::to_string(dbTime)},
            {"Building time", std::to_string(buildingTime)}};
        auto modelDescriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name,
                                                      metadata);
        scene.addModel(modelDescriptor);
        PLUGIN_TIMER(chrono3.elapsed(), "Model addition to the scene");
        PLUGIN_TIMER(chrono0.elapsed(), "Cells successfully loaded");
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::loadCellsAsSDF(brayns::Scene& scene, const std::string& name,
                                 const std::string& sqlCell,
                                 const std::string& sqlMorphology)
{
    PLUGIN_INFO << "Loading SDF cells from DB..." << std::endl;

    brayns::Timer chrono0;
    try
    {
        // Read morphology contents
        PLUGIN_DEBUG << sqlMorphology << std::endl;
        pqxx::read_transaction transaction(_connection);
        pqxx::result res = transaction.exec(sqlMorphology);

        auto model = scene.createModel();
        auto& sdfGeometries = model->getSDFGeometryData();
        const auto nbCells = res.size();
        sdfGeometries.geometries.resize(nbCells);

        brayns::Timer chrono1;
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const pqxx::binarystring sdfBUffer(c.at(1));
            auto indexInBuffer = sdfBUffer.data();

            auto& geometries = sdfGeometries.geometries;

            // Geometries
            uint64_t nbGeometries;
            memcpy(&nbGeometries, indexInBuffer, sizeof(uint64_t));
            indexInBuffer += sizeof(uint64_t);
            geometries.resize(nbGeometries);
            const auto geometriesBufferSize =
                nbGeometries * sizeof(brayns::SDFGeometry);
            memcpy(geometries.data(), indexInBuffer, geometriesBufferSize);
            indexInBuffer += geometriesBufferSize;

            // Indices
            uint64_t nbGeometryIndices;
            memcpy(&nbGeometryIndices, indexInBuffer, sizeof(uint64_t));
            indexInBuffer += sizeof(uint64_t);
            for (uint64_t i = 0; i < nbGeometryIndices; ++i)
            {
                brayns::PropertyMap props;
                props.setProperty({MATERIAL_PROPERTY_SHADING_MODE, 1});
                auto material =
                    model->createMaterial(i, std::to_string(i), props);
                material->setDiffuseColor({rand() % 100 / 100.f,
                                           rand() % 100 / 100.f,
                                           rand() % 100 / 100.f});
                material->setSpecularColor({0, 0, 0});

                uint64_t geometryIndexKey;
                memcpy(&geometryIndexKey, indexInBuffer, sizeof(size_t));
                indexInBuffer += sizeof(size_t);

                auto& geometryIndex =
                    sdfGeometries.geometryIndices[geometryIndexKey];

                uint64_t nbGeometryIndexSize;
                memcpy(&nbGeometryIndexSize, indexInBuffer, sizeof(uint64_t));
                indexInBuffer += sizeof(uint64_t);
                geometryIndex.resize(nbGeometryIndexSize);
                const uint64_t geometryIndexBufferSize =
                    nbGeometryIndexSize * sizeof(uint64_t);
                memcpy(geometryIndex.data(), indexInBuffer,
                       geometryIndexBufferSize);
                indexInBuffer += geometryIndexBufferSize;
            }

            // Neighbours
            uint64_t nbNeighbours;
            memcpy(&nbNeighbours, indexInBuffer, sizeof(uint64_t));
            indexInBuffer += sizeof(uint64_t);
            sdfGeometries.neighbours.resize(nbNeighbours);
            for (uint64_t i = 0; i < nbNeighbours; ++i)
            {
                auto& neighbour = sdfGeometries.neighbours[i];

                uint64_t neighbourSize;
                memcpy(&neighbourSize, indexInBuffer, sizeof(uint64_t));
                indexInBuffer += sizeof(uint64_t);

                if (neighbourSize > 0)
                {
                    neighbour.resize(neighbourSize);
                    const uint64_t neighbourBufferSize =
                        neighbourSize * sizeof(size_t);
                    memcpy(neighbour.data(), indexInBuffer,
                           neighbourBufferSize);
                    indexInBuffer += neighbourBufferSize;
                }
            }

            // Flat neighbours
            uint64_t nbFlatNeighbours;
            memcpy(&nbFlatNeighbours, indexInBuffer, sizeof(uint64_t));
            indexInBuffer += sizeof(uint64_t);
            sdfGeometries.neighboursFlat.resize(nbFlatNeighbours);
            const uint64_t flatNeighboursBufferSize =
                nbFlatNeighbours * sizeof(uint64_t);
            memcpy(sdfGeometries.neighboursFlat.data(), indexInBuffer,
                   flatNeighboursBufferSize);

            PLUGIN_INFO << "Successfully loaded SDF: " << nbGeometries
                        << " geometries, " << nbGeometryIndices << " indices, "
                        << nbNeighbours << " neighbours, " << nbFlatNeighbours
                        << " flat neighbours, and "
                        << model->getMaterials().size() << " materials"
                        << std::endl;
        }
        const auto dbTime = chrono1.elapsed();
        PLUGIN_TIMER(dbTime,
                     "Loading of " << nbCells << " unique cell contents");

        brayns::ModelMetadata metadata = {{"Number of cells",
                                           std::to_string(nbCells)},
                                          {"SQL for cells", sqlCell},
                                          {"SQL for morphologies",
                                           sqlMorphology},
                                          {"DB time", std::to_string(dbTime)}};
        auto modelDescriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name,
                                                      metadata);
        scene.addModel(modelDescriptor);
        PLUGIN_TIMER(chrono0.elapsed(), "Cells successfully loaded");
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::importMorphology(brayns::Scene& scene, const uint64_t guid,
                                   const std::string& filename)
{
    PLUGIN_INFO << "Importing morphology " << guid << " from " << filename
                << std::endl;
    brayns::PropertyMap props;
    props.setProperty({"070RealisticSoma", false});
    props.setProperty({"090MorphologyQuality", enumToString<MorphologyQuality>(
                                                   MorphologyQuality::high)});
    props.setProperty(
        {"022UserDataType", enumToString(UserDataType::undefined)});
    props.setProperty({"060UseSdfgeometry", false});
    props.setProperty({"061DampenBranchThicknessChangerate", false});
    props.setProperty(
        {"080MorphologyColorScheme", enumToString(CircuitColorScheme::none)});
    props.setProperty({"051RadiusCorrection", 0.});
    props.setProperty({"050RadiusMultiplier", 1.});
    props.setProperty({"052SectionTypeSoma", false});
    props.setProperty(
        {"091MaxDistanceToSoma", std::numeric_limits<double>::max()});

    pqxx::work transaction(_connection);

    const std::vector<std::string> columns = {"axon", "dendrite",
                                              "apicaldendrite"};
    for (size_t section = 0; section < columns.size(); ++section)
    {
        try
        {
            props.setProperty({"053SectionTypeAxon", section == 0});
            props.setProperty({"054SectionTypeDendrite", section == 1});
            props.setProperty({"055SectionTypeApicalDendrite", section == 2});

            MorphologyLoader loader(scene, std::move(props));
            auto modelDescriptor =
                loader.importFromFile(filename, brayns::LoaderProgress(),
                                      props);

            auto& model = modelDescriptor->getModel();

            auto& spheres = model.getSpheres()[0];
            uint64_t nbSpheres = spheres.size();
            auto& cylinders = model.getCylinders()[0];
            uint64_t nbCylinders = cylinders.size();
            auto& cones = model.getCones()[0];
            uint64_t nbCones = cones.size();

            const auto bufferSize =
                sizeof(uint64_t) + sizeof(brayns::Sphere) * nbSpheres +
                sizeof(uint64_t) + sizeof(brayns::Cylinder) * nbCylinders +
                sizeof(uint64_t) + sizeof(brayns::Cone) * nbCones;

            std::vector<uint8_t> buffer(bufferSize);
            auto index = buffer.data();
            memcpy(index, &nbSpheres, sizeof(uint64_t));
            index += sizeof(uint64_t);
            if (nbSpheres > 0)
            {
                memcpy(index, spheres.data(),
                       sizeof(brayns::Sphere) * nbSpheres);
                index += sizeof(brayns::Sphere) * nbSpheres;
            }

            memcpy(index, &nbCylinders, sizeof(uint64_t));
            index += sizeof(uint64_t);
            if (nbCylinders > 0)
            {
                memcpy(index, cylinders.data(),
                       sizeof(brayns::Cylinder) * nbCylinders);
                index += sizeof(brayns::Cylinder) * nbCylinders;
            }

            memcpy(index, &nbCones, sizeof(uint64_t));
            index += sizeof(uint64_t);
            if (nbCones > 0)
                memcpy(index, cones.data(), sizeof(brayns::Cone) * nbCones);

            pqxx::binarystring data((void*)buffer.data(), bufferSize);
            transaction
                .parameterized("UPDATE morphology SET " + columns[section] +
                               "=$1 WHERE guid=$2")(data)(guid)
                .exec();
            transaction.commit();

            PLUGIN_INFO << "Successfully imported " << nbSpheres << " spheres, "
                        << nbCylinders << " cylinders and " << nbCones
                        << " cones" << std::endl;
        }
        catch (const std::runtime_error& e)
        {
            transaction.abort();
            PLUGIN_THROW(e.what());
        }
    }
}

void DBConnector::importMorphologyAsSDF(brayns::Scene& scene,
                                        const uint64_t guid,
                                        const std::string& filename)
{
    PLUGIN_INFO << "Importing morphology " << guid << " from " << filename
                << std::endl;
    brayns::PropertyMap props;
    props.setProperty({"070RealisticSoma", false});
    props.setProperty({"090MorphologyQuality", enumToString<MorphologyQuality>(
                                                   MorphologyQuality::high)});
    props.setProperty(
        {"022UserDataType", enumToString(UserDataType::distance_to_soma)});
    props.setProperty({"060UseSdfgeometry", true});
    props.setProperty({"061DampenBranchThicknessChangerate", true});
    props.setProperty(
        {"080MorphologyColorScheme", enumToString(CircuitColorScheme::none)});
    props.setProperty({"051RadiusCorrection", 0.});
    props.setProperty({"050RadiusMultiplier", 1.});
    props.setProperty({"052SectionTypeSoma", true});
    props.setProperty(
        {"091MaxDistanceToSoma", std::numeric_limits<double>::max()});

    pqxx::work transaction(_connection);
    try
    {
        props.setProperty({"053SectionTypeAxon", true});
        props.setProperty({"054SectionTypeDendrite", true});
        props.setProperty({"055SectionTypeApicalDendrite", true});

        MorphologyLoader loader(scene, std::move(props));
        auto modelDescriptor =
            loader.importFromFile(filename, brayns::LoaderProgress(), props);

        auto& model = modelDescriptor->getModel();

        auto& sdfGeometry = model.getSDFGeometryData();

        // Geometries
        const uint64_t nbGeometries = sdfGeometry.geometries.size();
        const uint64_t geometriesBufferSize =
            sizeof(brayns::SDFGeometry) * nbGeometries;

        // Indices
        const uint64_t nbGeometryIndices = sdfGeometry.geometryIndices.size();
        uint64_t geometryIndicesSize{0};
        for (const auto& geometryIndex : sdfGeometry.geometryIndices)
        {
            // Map key
            geometryIndicesSize += sizeof(size_t);
            // Map value (Length + buffer)
            geometryIndicesSize += sizeof(uint64_t);
            geometryIndicesSize +=
                geometryIndex.second.size() * sizeof(uint64_t);
        }

        // Neighbours
        uint64_t neighboursSize{0};
        const uint64_t nbNeighbours = sdfGeometry.neighbours.size();
        for (const auto& neighbour : sdfGeometry.neighbours)
        {
            geometryIndicesSize += sizeof(uint64_t);
            neighboursSize += neighbour.size() * sizeof(size_t);
        }

        // Flat neighbours
        const uint64_t nbNeighboursFlat = sdfGeometry.neighboursFlat.size();
        const uint64_t neighboursFlatSize =
            sizeof(uint64_t) * sdfGeometry.neighboursFlat.size();

        const auto bufferSize = sizeof(uint64_t) + geometriesBufferSize +
                                sizeof(uint64_t) + geometryIndicesSize +
                                sizeof(uint64_t) + neighboursSize +
                                sizeof(uint64_t) + neighboursFlatSize;

        // Create DB buffer
        std::vector<uint8_t> buffer(bufferSize);

        // Geometries
        auto index = buffer.data();
        memcpy(index, &nbGeometries, sizeof(uint64_t));
        index += sizeof(uint64_t);
        if (geometriesBufferSize > 0)
        {
            memcpy(index, sdfGeometry.geometries.data(), geometriesBufferSize);
            index += geometriesBufferSize;
        }

        // Indices
        memcpy(index, &nbGeometryIndices, sizeof(uint64_t));
        index += sizeof(uint64_t);
        if (nbGeometryIndices > 0)
            for (const auto& geometryIndex : sdfGeometry.geometryIndices)
            {
                const size_t key = geometryIndex.first;
                memcpy(index, &key, sizeof(size_t));
                index += sizeof(size_t);

                const uint64_t nbElements = geometryIndex.second.size();
                memcpy(index, &nbElements, sizeof(nbElements));
                index += sizeof(nbElements);

                const uint64_t geometryIndexBufferSize =
                    geometryIndex.second.size() * sizeof(uint64_t);
                memcpy(index, geometryIndex.second.data(),
                       geometryIndexBufferSize);
                index += geometryIndexBufferSize;
            }

        // Neighbours
        memcpy(index, &nbNeighbours, sizeof(uint64_t));
        index += sizeof(uint64_t);
        for (const auto& neighbour : sdfGeometry.neighbours)
        {
            const uint64_t neighbourSize = neighbour.size();
            memcpy(index, &neighbourSize, sizeof(uint64_t));
            index += sizeof(uint64_t);
            if (neighbourSize > 0)
            {
                const uint64_t neighbourBufferSize =
                    neighbourSize * sizeof(size_t);
                memcpy(index, neighbour.data(), neighbourBufferSize);
                index += neighbourBufferSize;
            }
        }

        // Flat neighbours
        memcpy(index, &neighboursFlatSize, sizeof(uint64_t));
        index += sizeof(uint64_t);
        if (neighboursFlatSize > 0)
        {
            const uint64_t neighboursFlatBufferSize =
                neighboursFlatSize * sizeof(uint64_t);
            memcpy(index, sdfGeometry.neighboursFlat.data(),
                   neighboursFlatBufferSize);
        }

        PLUGIN_INFO << bufferSize << " vs " << index - buffer.data()
                    << std::endl;
        const pqxx::binarystring data((void*)buffer.data(), bufferSize);
        transaction
            .parameterized("UPDATE morphology SET sdf=$1 WHERE guid=$2")(data)(
                guid)
            .exec();
        transaction.commit();

        PLUGIN_INFO << "Successfully imported SDF: " << nbGeometries
                    << " geometries, " << nbGeometryIndices << " indices, "
                    << nbNeighbours << " neighbours, and " << nbNeighboursFlat
                    << " flat neighbours" << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        transaction.abort();
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::importVolume(const uint64_t guid,
                               const brayns::Vector3f& dimensions,
                               const brayns::Vector3f& spacing,
                               const std::string& filename)
{
    pqxx::work transaction(_connection);
    try
    {
        std::ifstream file(filename, std::fstream::binary);
        if (!file.good())
            PLUGIN_THROW("Could not open volume file");

        const std::ifstream::pos_type pos = file.tellg();
        std::vector<char> buffer(pos);
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), pos);
        file.close();

        const pqxx::binarystring voxels((void*)buffer.data(), buffer.size());
        transaction
            .parameterized(
                "INSERT INTO volume VALUES "
                "($1, ARRAY[$2,$3,$4], ARRAY[$5,$6,$7], $8)")(guid)(
                dimensions.x)(dimensions.y)(dimensions.z)(spacing.x)(spacing.y)(
                spacing.z)(voxels)
            .exec();
        transaction.commit();
    }
    catch (const std::runtime_error& e)
    {
        transaction.abort();
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::loadMeshes(brayns::Scene& scene,
                             const std::string& sqlStatement)
{
    PLUGIN_INFO << "Loading meshes" << std::endl;

    try
    {
        pqxx::read_transaction transaction(_connection);
        const auto sql = sqlStatement.empty()
                             ? "SELECT m.type, m.contents, r.code FROM meshes "
                               "AS m, region AS r WHERE m.guid=r.guid"
                             : sqlStatement;
        auto res = transaction.exec(sql);
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            brayns::MeshLoader loader(scene);

            const auto meshType = c[0].as<std::string>();
            const pqxx::binarystring s(c[1]);
            const auto name = c[2].as<std::string>();

            brayns::Blob blob{meshType, name, {}};
            blob.data.resize(s.size());
            memcpy(&blob.data[0], s.data(), s.size());
            try
            {
                auto modelDescriptor =
                    loader.importFromBlob(std::move(blob),
                                          brayns::LoaderProgress(),
                                          brayns::PropertyMap());
                brayns::Transformation tf;
                tf.setScale({5.f, 5.f, 5.f});
                modelDescriptor->setTransformation(tf);
                scene.addModel(modelDescriptor);
            }
            catch (const std::runtime_error& e)
            {
                PLUGIN_ERROR << e.what() << std::endl;
            }
        }
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::loadSomas(brayns::Scene& scene, const std::string& name,
                            const std::string& sqlStatement, const float radius)
{
    PLUGIN_INFO << "Loading somas" << std::endl;
    const auto cells = getCells(sqlStatement);
    PLUGIN_INFO << "Loading " << cells.size() << " somas" << std::endl;

    if (cells.empty())
        PLUGIN_THROW("No cells match the requested statement");

    PLUGIN_INFO << "Creating model with " << cells.size() << " cells"
                << std::endl;
    auto model = scene.createModel();
    if (!model)
        PLUGIN_THROW("Failed to create model");

    const auto materialId = 0;
    brayns::PropertyMap props;
    props.setProperty({MATERIAL_PROPERTY_SHADING_MODE, 0});
    auto material =
        model->createMaterial(materialId, std::to_string(materialId), props);
    material->setDiffuseColor(
        {rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f});
    material->setSpecularColor({0, 0, 0});

    for (const auto& cell : cells)
        model->addSphere(materialId,
                         {cell.transformation.getTranslation(), radius});

    brayns::ModelMetadata metadata = {{"Number of cells",
                                       std::to_string(cells.size())},
                                      {"SQL", sqlStatement}};
    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), name,
                                                  metadata);
    scene.addModel(modelDescriptor);
}

void DBConnector::loadSegments(brayns::Scene& scene, const std::string& name,
                               const std::string& sqlStatement,
                               const float radius)
{
    PLUGIN_INFO << "Loading segments" << std::endl;

    try
    {
        pqxx::read_transaction transaction(_connection);
        const auto sql = sqlStatement.empty()
                             ? "SELECT segment_id, points FROM cell_segments"
                             : sqlStatement;

        auto res = transaction.exec(sql);
        auto model = scene.createModel();
        if (!model)
            PLUGIN_THROW("Failed to create model");

        const auto materialId = 0;
        brayns::PropertyMap props;
        props.setProperty({MATERIAL_PROPERTY_SHADING_MODE, 0});
        auto material =
            model->createMaterial(materialId, std::to_string(materialId),
                                  props);
        material->setDiffuseColor(
            {rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f});
        material->setSpecularColor({0, 0, 0});

        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const pqxx::binarystring points(c[1]);
            if (points.size() % 24 != 0)
                PLUGIN_THROW("Invalid number of values in segments definition");

            const auto nbSegments =
                points.size() / (6 * sizeof(brayns::Vector3f));

            PLUGIN_INFO << "Loading " << nbSegments << " segments" << std::endl;
            for (uint64_t i = 0; i < nbSegments; ++i)
            {
                const uint64_t indexOrigin = i * 6 * sizeof(float);
                const uint64_t indexTarget = (i + 3) * 6 * sizeof(float);
                brayns::Vector3f origin;
                brayns::Vector3f target;
                memcpy(&origin, &points[indexOrigin], sizeof(brayns::Vector3f));
                memcpy(&target, &points[indexTarget], sizeof(brayns::Vector3f));
                model->addCylinder(materialId, {origin, target, radius});
            }
        }

        brayns::ModelMetadata metadata = {{"SQL", sqlStatement}};
        auto modelDescriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name,
                                                      metadata);
        scene.addModel(modelDescriptor);
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::loadSynapses(brayns::Scene& scene, const std::string& name,
                               const std::string& sqlStatement,
                               const float radius)
{
    PLUGIN_INFO << "Loading synapses" << std::endl;

    try
    {
        pqxx::read_transaction transaction(_connection);
        const auto sql = sqlStatement.empty()
                             ? "SELECT segment_id, points FROM cell_segments"
                             : sqlStatement;

        auto res = transaction.exec(sql);
        auto model = scene.createModel();
        if (!model)
            PLUGIN_THROW("Failed to create model");

        const auto materialId = 0;
        brayns::PropertyMap props;
        props.setProperty({MATERIAL_PROPERTY_SHADING_MODE, 0});
        auto material =
            model->createMaterial(materialId, std::to_string(materialId),
                                  props);
        material->setDiffuseColor(
            {rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f});
        material->setSpecularColor({0, 0, 0});

        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const pqxx::binarystring points(c[1]);
            if (points.size() % 24 != 0)
                PLUGIN_THROW("Invalid number of values in segments definition");

            const auto nbSegments =
                points.size() / (6 * sizeof(brayns::Vector3f));

            PLUGIN_INFO << "Loading " << nbSegments << " segments" << std::endl;
            for (uint64_t i = 0; i < nbSegments; ++i)
            {
                const uint64_t indexOrigin = i * 6 * sizeof(float);
                const uint64_t indexTarget = (i + 3) * 6 * sizeof(float);
                brayns::Vector3f origin;
                brayns::Vector3f target;
                memcpy(&origin, &points[indexOrigin], sizeof(brayns::Vector3f));
                memcpy(&target, &points[indexTarget], sizeof(brayns::Vector3f));
                model->addCylinder(materialId, {origin, target, radius});
            }
        }

        brayns::ModelMetadata metadata = {{"SQL", sqlStatement}};
        auto modelDescriptor =
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name,
                                                      metadata);
        scene.addModel(modelDescriptor);
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
}

void DBConnector::loadCellOrientations(brayns::Scene& scene,
                                       const std::string& name,
                                       const std::string& sqlStatement,
                                       const float radius)
{
    PLUGIN_INFO << "Loading cell orientations" << std::endl;
    const auto cells = getCells(sqlStatement);
    PLUGIN_INFO << "Loading " << cells.size() << " somas" << std::endl;

    if (cells.empty())
        PLUGIN_THROW("No cells match the requested statement");

    PLUGIN_INFO << "Creating model with " << cells.size() << " cells"
                << std::endl;
    auto model = scene.createModel();
    if (!model)
        PLUGIN_THROW("Failed to create model");

    uint64_t materialId = 0;
    const brayns::Vector3f up{0.f, 0.f, -1.f};
    for (const auto& cell : cells)
    {
        const auto& position = cell.transformation.getTranslation();
        const auto& rotation = cell.transformation.getRotation();
        const auto target =
            position + 20.f * radius * up * glm::toMat3(rotation);

        brayns::PropertyMap props;
        props.setProperty({MATERIAL_PROPERTY_SHADING_MODE, 1});
        auto material =
            model->createMaterial(materialId, std::to_string(materialId),
                                  props);
        const auto color = glm::normalize(target - position);
        material->setDiffuseColor({0.5f + color.x * 0.5f, 0.5f + color.y * 0.5f,
                                   0.5f + color.z * 0.5f});
        material->setSpecularColor({0, 0, 0});

        model->addSphere(materialId, {position, radius});
        model->addCone(materialId, {position, target, radius, 0.f});
        ++materialId;
    }

    brayns::ModelMetadata metadata = {
        {"Number of cells", std::to_string(cells.size())}};
    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), name,
                                                  sqlStatement, metadata);
    scene.addModel(modelDescriptor);
}

#if 0
std::vector<float> DBConnector::loadSomaSimulationFromDB(
    const uint64_t frame, const std::string& sqlStatement)
{
    std::vector<float> values;
    const uint64_t nbGuids{137300};
    values.resize(nbGuids);
    const uint64_t guidStep = nbGuids / NB_CONNECTIONS;

    const brayns::Timer chrono0;
    try
    {
        uint64_t guid = 0;
#pragma omp parallel for num_threads(NB_CONNECTIONS)
        for (guid = 0; guid < nbGuids; guid += guidStep)
        {
            const auto sql = sqlStatement.empty()
                                 ? "SELECT get_byte(values," +
                                       std::to_string(frame) +
                                       ") FROM simulation_somas WHERE guid>=" +
                                       std::to_string(guid) + " AND guid<" +
                                       std::to_string(guid + guidStep)
                                 : sqlStatement;

            pqxx::work transaction(
                *_connections[omp_get_thread_num()]);
            auto res = transaction.exec(sql);
            uint64_t index{0};
            for (auto c = res.begin(); c != res.end(); ++c)
            {
                values[guid + index] = float(c[0].as<int>()) - 100.f;
                ++index;
            }
        }
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_ERROR << e.what() << std::endl;
    }
    PLUGIN_TIMER(chrono0.elapsed(), "loadSomaSimulationFromDB");
    return values;
}
#else
std::vector<float> DBConnector::loadSomaSimulation(
    const uint64_t frame, const std::string& sqlStatement)
{
    std::vector<float> values;
    const uint64_t nbGuids{137300};
    values.resize(nbGuids);

    const brayns::Timer chrono0;
    try
    {
        const auto sql =
            sqlStatement.empty()
                ? "SELECT voltages FROM simulation_somas_2 WHERE frame=" +
                      std::to_string(frame)
                : sqlStatement;

        pqxx::work transaction(_connection);
        auto res = transaction.exec(sql);
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const pqxx::binarystring voltages(c[0]);
            for (uint64_t i = 0; i < values.size(); ++i)
                values[i] = float(voltages.data()[i]) - 100.f;
        }
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
    PLUGIN_TIMER(chrono0.elapsed(), "loadSomaSimulationFromDB");
    return values;
}
#endif

void DBConnector::importCompartmentSimulation(const std::string blueConfig,
                                              const std::string reportName)
{
    std::vector<pqxx::connection*> connections;
    for (size_t i = 0; i < NB_CONNECTIONS; ++i)
    {
        PLUGIN_INFO << "Initializing connection " << i << ": "
                    << _connectionString << std::endl;
        connections.push_back(new pqxx::connection(_connectionString));
        PLUGIN_INFO << (connections[i] ? "OK" : "KO") << std::endl;
    }

    PLUGIN_INFO << "Import compartment simulation" << std::endl;
    const brion::BlueConfig bc(blueConfig);
    brion::CompartmentReport compartmentReport(bc.getReportSource(reportName),
                                               brion::MODE_READ);
    const auto guids = compartmentReport.getGIDs();
    const auto dt = compartmentReport.getTimestep();
    const auto nbFrames = compartmentReport.getEndTime() / dt;

    PLUGIN_INFO << "Importing " << guids.size() << " guids" << std::endl;

    std::vector<uint32_t> guidList;
    for (const auto guid : guids)
        guidList.push_back(guid);

    uint64_t i;
#pragma omp parallel for num_threads(NB_CONNECTIONS)
    for (i = 0; i < guids.size(); ++i)
    {
        const auto guid = guidList[i];
        const brain::GIDSet singleGuid{guid};
        const brion::CompartmentReport report(bc.getReportSource(reportName),
                                              brion::MODE_READ, singleGuid);

        try
        {
            pqxx::work transaction(*connections[omp_get_thread_num()]);
            for (uint64_t frame = 0; frame < nbFrames; ++frame)
            {
                const auto voltages = *report.loadFrame(frame * dt).get().data;

                const pqxx::binarystring data((void*)voltages.data(),
                                              voltages.size() * sizeof(float));
                transaction
                    .parameterized(
                        "INSERT INTO simulation_compartments VALUES "
                        "($1,$2,$3)")(guid)(frame)(data)
                    .exec();
            }
            transaction.commit();
        }
        catch (pqxx::sql_error& e)
        {
            PLUGIN_ERROR << e.what() << std::endl;
        }
    }

    for (i = 0; i < NB_CONNECTIONS; ++i)
        connections[i]->disconnect();
}

std::map<uint64_t, std::vector<float>> DBConnector::loadSomaSimulation()
{
    std::map<uint64_t, std::vector<float>> frames;

    pqxx::work transaction(_connection);
    try
    {
        const brayns::Timer chrono0;
        const auto sql = "SELECT guid, values FROM simulation_somas";

        auto res = transaction.exec(sql);
        PLUGIN_TIMER(chrono0.elapsed(), sql);

        const brayns::Timer chrono1;
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const auto guid = c[0].as<uint64_t>();
            frames[guid].reserve(35000);
            const pqxx::binarystring values(c[1]);
            for (uint64_t i = 0; i < 35000; ++i)
                frames[guid].push_back(float(values[i]) - 100.f);
        }
        PLUGIN_TIMER(chrono1.elapsed(), "Frame building");
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
    transaction.abort();
    return frames;
}
