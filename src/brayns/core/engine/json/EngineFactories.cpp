/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "EngineFactories.h"

#include "adapters/GeometryAdapters.h"
#include "adapters/LightAdapters.h"
#include "adapters/MaterialAdapters.h"
#include "adapters/ProjectionAdapters.h"
#include "adapters/RendererAdapters.h"
#include "adapters/VolumeAdapter.h"

namespace
{
class FactoryInitialization
{
public:
    static void initCameras(brayns::EngineFactory<brayns::Camera> &factory)
    {
        factory.addType<brayns::Orthographic>();
        factory.addType<brayns::Perspective>();
    }

    static void initGeometries(brayns::EngineFactory<brayns::Geometry> &factory)
    {
        factory.addType<brayns::Box>();
        factory.addType<brayns::Capsule>();
        factory.addType<brayns::Plane>();
        factory.addType<brayns::Sphere>();
        factory.addType<brayns::TriangleMesh>();
    }

    static void initLights(brayns::EngineFactory<brayns::Light> &factory)
    {
        factory.addType<brayns::AmbientLight>();
        factory.addType<brayns::DirectionalLight>();
        factory.addType<brayns::QuadLight>();
        factory.addType<brayns::SphereLight>();
    }

    static void initMaterials(brayns::EngineFactory<brayns::Material> &factory)
    {
        factory.addType<brayns::CarPaint>();
        factory.addType<brayns::Emissive>();
        factory.addType<brayns::Glass>();
        factory.addType<brayns::Matte>();
        factory.addType<brayns::Metal>();
        factory.addType<brayns::Phong>();
        factory.addType<brayns::Plastic>();
    }

    static void initRenderers(brayns::EngineFactory<brayns::Renderer> &factory)
    {
        factory.addType<brayns::Interactive>();
        factory.addType<brayns::Production>();
    }

    static void initVolumes(brayns::EngineFactory<brayns::Volume> &factory)
    {
        factory.addType<brayns::RegularVolume>();
    }
};
}

namespace brayns
{
void EngineFactoriesInitializer::init(EngineFactories &factories)
{
    FactoryInitialization::initCameras(factories.cameras);
    FactoryInitialization::initGeometries(factories.geometries);
    FactoryInitialization::initLights(factories.lights);
    FactoryInitialization::initMaterials(factories.materials);
    FactoryInitialization::initRenderers(factories.renderer);
    FactoryInitialization::initVolumes(factories.volumes);
}
}
