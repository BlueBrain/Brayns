/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include "AbstractParameters.h"

#include <stdio.h>

namespace brayns
{

/** Command line parameter data types
 */
enum SceneEnvironment
{
    SE_NONE = 0,
    SE_GROUND,
    SE_WALL,
    SE_BOX
};

/** Manages geometry parameters
 */
class GeometryParameters : public AbstractParameters
{
public:
    GeometryParameters();

    /** Parse the command line parameters and populates according class members
     *
     * @param argc number of command line parameters
     * @param argv actual command line parameters
     */
    void parse(int argc, const char **argv) final;

    /** Displays parameters managed by the class
     */
    void display() const final;

    /** folder containing SWC files */
    std::string getSWCFolder() const { return _swcFolder; }

    /** folder containing PDB files */
    std::string getPDBFolder() const { return _pdbFolder; }

    /** folder containing PDB cells */
    std::string getPDBCells() const { return _pdbCells; }

    /** folder containing PDB positions */
    std::string getPDBPositions() const { return _pdbPositions; }

    /** folder containing H5 files */
    std::string getH5Folder() const { return _h5Folder; }

    /** folder containing mesh files */
    std::string getMeshFolder() const { return _meshFolder; }

    /** Radius multiplier applied to spheres, cones and cylinders */
    float getRadius() const { return _radius; }
    void setRadius(float value) { _radius = value; }

    /** Enables a different color for every morphology/mesh when
    * loading them from a given folder
    */
    bool getColored() const { return _colored; }
    void setColored(bool value) { _colored = value; }

    /** Enables timed geometries by incrementing the timestamp for
     * every computer frame */
    bool getTimedGeometry() const { return _timedGeometry; }
    void setTimedGeometry(bool value) { _timedGeometry = value; }

    /** Defines the timestamp increment between two frames */
    size_t getTimedGeometryIncrement() const
    { return _timedGeometryIncrement; }
    void setTimedGeometryIncrement( size_t value)
    { _timedGeometryIncrement = value; }

    /** Scene environment (0: none, 1: ground, 2: box ) */
    SceneEnvironment getSceneEnvironment() const { return _sceneEnvironment; }

protected:
    std::string _swcFolder;
    std::string _pdbFolder;
    std::string _pdbCells;
    std::string _pdbPositions;
    std::string _h5Folder;
    std::string _meshFolder;
    float  _radius;
    bool   _colored;
    bool   _timedGeometry;
    size_t _timedGeometryIncrement;
    SceneEnvironment _sceneEnvironment;
};

}
#endif // GEOMETRYPARAMETERS_H
