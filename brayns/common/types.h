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

#ifndef TYPES_H
#define TYPES_H

#include <brayns/common/mathTypes.h>

#include <memory>
#include <map>
#include <vector>

namespace brayns
{

// Forward declarations
class Brayns;
typedef std::unique_ptr<Brayns> BraynsPtr;

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class Renderer;
typedef std::shared_ptr<Renderer> RendererPtr;

class FrameBuffer;
typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

class ApplicationParameters;
typedef std::shared_ptr<ApplicationParameters> ApplicationParametersPtr;

class GeometryParameters;
typedef std::shared_ptr<GeometryParameters> GeometryParametersPtr;

class RenderingParameters;
typedef std::shared_ptr<RenderingParameters> RenderingParametersPtr;

class ExtensionController;
typedef std::shared_ptr<ExtensionController> ExtensionControllerPtr;

class Geometry;
typedef std::vector<Geometry *> Geometries;

class Primitive;
typedef std::shared_ptr<Primitive> PrimitivePtr;
typedef std::vector<PrimitivePtr> Primitives;
typedef std::map<size_t, Primitives> PrimitivesCollection;

class Sphere;
typedef std::shared_ptr<Sphere> SpherePtr;
typedef std::vector<SpherePtr> Spheres;
typedef std::map<size_t, Spheres> SpheresCollection;

class Cylinder;
typedef std::shared_ptr<Cylinder> CylinderPtr;
typedef std::vector<CylinderPtr> Cylinders;
typedef std::map<size_t, Cylinders> CylindersCollection;

class Cone;
typedef std::shared_ptr<Cone> ConePtr;
typedef std::vector<ConePtr> Cones;
typedef std::map<size_t, Cones> ConesCollection;

class TrianglesMesh;
typedef std::map<size_t, TrianglesMesh> TrianglesMeshCollection;

class Material;
typedef std::shared_ptr<Material> MaterialPtr;
typedef std::vector<MaterialPtr> Materials;
typedef std::map<size_t, Materials> MaterialsCollection;

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;
typedef std::map<std::string, Texture2DPtr> TexturesCollection;

typedef std::vector< std::string > strings;
typedef std::vector< float > floats;
typedef std::vector< int > ints;
typedef std::vector< uint8_t > uint8_ts;

class ParametersManager;
class AbstractParameters;
class ApplicationParameters;
class GeometryParameters;
class RenderingParameters;

/** Define the color scheme to be applied to the geometry */
enum ColorScheme
{
    CS_NONE,
    CS_PROTEIN_ATOMS,
    CS_PROTEIN_CHAINS,
    CS_PROTEIN_RESIDUES,
    CS_PROTEIN_BACKBONE,
    CS_NEURON_DEFAULT,
    CS_NEURON_BY_ID,
    CS_NEURON_BY_TYPE,
    CS_NEURON_BY_SEGMENT_TYPE,
};

/** Define the environment that is added to the default scene */
enum SceneEnvironment
{
    SE_NONE = 0,
    SE_GROUND,
    SE_WALL,
    SE_BOX
};

}

#endif // TYPES_H
