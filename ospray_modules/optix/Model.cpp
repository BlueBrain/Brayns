/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

// ospray
#include "Model.h"

#include "Context.h"

#include "render/Material.h"

#include <ospray/SDK/common/OSPCommon.h>
#include <ospray/SDK/common/Util.h>

namespace bbp
{
namespace optix
{
const std::string DEFAULT_ACCELERATION_STRUCTURE = "Trbvh";

Model::Model()
{
    managedObjectType = OSP_MODEL;
    _context = Context::get().getOptixContext();
}

Model::~Model()
{
    for (auto& i : _geometryInstances)
        i->destroy();
    if (_geometryGroup)
        _geometryGroup->destroy();
}

std::string Model::toString() const
{
    return "ospray::Model";
}

void Model::commit()
{
    ospray::postStatusMsg(2)
        << "=======================================================\n"
        << "Finalizing model, has " << geometry.size()
        << " geometries and 0 volumes";

    for (auto& i : _geometryInstances)
        i->destroy();
    _geometryInstances.clear();

    if (_geometryGroup)
        _geometryGroup->destroy();
    _geometryGroup = nullptr;

    for (size_t i = 0; i < geometry.size(); i++)
    {
        ospray::postStatusMsg(2)
            << "=======================================================\n"
            << "Finalizing geometry " << i;

        geometry[i]->finalize(this);
    }

    _geometryGroup = _context->createGeometryGroup();
    _geometryGroup->setAcceleration(
        _context->createAcceleration(DEFAULT_ACCELERATION_STRUCTURE,
                                     DEFAULT_ACCELERATION_STRUCTURE));
    _geometryGroup->setChildCount(_geometryInstances.size());
    for (size_t i = 0; i < _geometryInstances.size(); ++i)
        _geometryGroup->setChild(i, _geometryInstances[i]);

    _context["top_object"]->set(_geometryGroup);
    _context["top_shadower"]->set(_geometryGroup);
}

Model* Model::createInstance(const char* type)
{
    return ospray::createInstanceHelper<Model, OSP_MODEL>(type);
}

void Model::addGeometryInstance(::optix::Geometry geometry_,
                                ::optix::Material material)
{
    ::optix::GeometryInstance geomInstance = _context->createGeometryInstance();
    geomInstance->setGeometry(geometry_);
    geomInstance->setMaterialCount(1);
    geomInstance->setMaterial(0, material);
    _geometryInstances.push_back(geomInstance);
}
}
}

namespace ospray
{
extern "C" ::bbp::optix::Model* ospray_create_model__()
{
    return new ::bbp::optix::Model;
}
::bbp::optix::Model* ospray_create_model__();
}
