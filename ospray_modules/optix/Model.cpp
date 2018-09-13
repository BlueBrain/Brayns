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

    if (_rootGroup)
        _rootGroup->destroy();

    if (_geometryGroupAcceleration)
        _geometryGroupAcceleration->destroy();

    if (_rootGroupAcceleration)
        _rootGroupAcceleration->destroy();

    if (getParam1i("isRootModel", false))
    {
        _context["top_object"] = nullptr;
        _context["top_shadower"] = nullptr;
    }
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

    if (_geometryGroupAcceleration)
        _geometryGroupAcceleration->destroy();

    _geometryGroupAcceleration =
        _context->createAcceleration(DEFAULT_ACCELERATION_STRUCTURE,
                                     DEFAULT_ACCELERATION_STRUCTURE);

    _geometryGroup = _context->createGeometryGroup();
    _geometryGroup->setAcceleration(_geometryGroupAcceleration);

    if (_rootGroup)
        _rootGroup->destroy();

    if (_rootGroupAcceleration)
        _rootGroupAcceleration->destroy();

    _rootGroupAcceleration =
        _context->createAcceleration(DEFAULT_ACCELERATION_STRUCTURE,
                                     DEFAULT_ACCELERATION_STRUCTURE);

    _rootGroup = _context->createGroup();
    _rootGroup->setAcceleration(_rootGroupAcceleration);

    _rootGroup->setChildCount(1);
    _rootGroup->setChild(0, _geometryGroup);

    for (size_t i = 0; i < geometry.size(); i++)
    {
        ospray::postStatusMsg(2)
            << "=======================================================\n"
            << "Finalizing geometry " << i;

        geometry[i]->finalize(this);
    }

    _geometryGroup->setChildCount(_geometryInstances.size());
    for (size_t i = 0; i < _geometryInstances.size(); ++i)
        _geometryGroup->setChild(i, _geometryInstances[i]);

    if (getParam1i("isRootModel", false))
    {
        _context["top_object"]->set(_rootGroup);
        _context["top_shadower"]->set(_rootGroup);
    }
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

void Model::addTransformInstance(::optix::Transform instance)
{
    const size_t count = _rootGroup->getChildCount();
    _rootGroup->setChildCount(count + 1);
    _rootGroup->setChild(count, instance);
}

::optix::Group Model::getRootGroup() const
{
    return _rootGroup;
}
}
}
