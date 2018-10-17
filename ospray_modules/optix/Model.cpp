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

    if (getParam1i("isRootModel", false))
    {
        if (_rootGroup)
            _rootGroup->destroy();

        if (_rootGroupAcceleration)
            _rootGroupAcceleration->destroy();

        _rootGroupAcceleration =
            _context->createAcceleration(DEFAULT_ACCELERATION_STRUCTURE,
                                         DEFAULT_ACCELERATION_STRUCTURE);

        _rootGroup = _context->createGroup();
        _rootGroup->setAcceleration(_rootGroupAcceleration);

        _context["top_object"]->set(_rootGroup);
        _context["top_shadower"]->set(_rootGroup);
    }
    else
    {
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
    }

    for (size_t i = 0; i < geometry.size(); i++)
    {
        ospray::postStatusMsg(2)
            << "=======================================================\n"
            << "Finalizing geometry " << i;

        geometry[i]->finalize(this);
    }


    if (getParam1i("isRootModel", false))
    {
        _context["top_object"]->set(_rootGroup);
        _context["top_shadower"]->set(_rootGroup);
    }
}

void Model::addGeometryInstance(const ::optix::GeometryInstance& instance)
{
    assert(_geometryGroup);

    _geometryInstances.push_back(instance);

    _geometryGroup->setChildCount(_geometryInstances.size());
    _geometryGroup->setChild(_geometryInstances.size() - 1,
                             _geometryInstances.back());
}

void Model::addTransformInstance(const ::optix::Transform& instance)
{
    assert(_rootGroup);

    const size_t count = _rootGroup->getChildCount();
    _rootGroup->setChildCount(count + 1);
    _rootGroup->setChild(count, instance);
}

::optix::Transform Model::instance(::optix::Matrix4x4&& matrix)
{
    assert(!_rootGroup);

    auto transform = _context->createTransform();
    transform->setChild(_geometryGroup);
    transform->setMatrix(false, matrix.getData(), matrix.inverse().getData());

    return transform;
}
}
}
