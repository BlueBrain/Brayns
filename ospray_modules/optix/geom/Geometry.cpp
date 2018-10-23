// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "Geometry.h"

#include "../Context.h"
#include "../Model.h"
#include "../render/Material.h"

#include <ospray/SDK/common/Util.h>

namespace bbp
{
namespace optix
{
Geometry::Geometry(const Type type)
    : _type(type)
{
    managedObjectType = OSP_GEOMETRY;
    _context = Context::get().getOptixContext();
}

Geometry::~Geometry()
{
    if (_geometry)
        _geometry->destroy();

    if (_buffer)
        _buffer->destroy();
}

void Geometry::setMaterial(ospray::Material* mat)
{
    if (!mat)
    {
        ospray::postStatusMsg()
            << "#osp: warning - tried to set NULL material; ignoring"
            << "#osp: warning. (note this means that object may not "
            << " get any material at all!)";
        return;
    }

    _material = mat;
    _bindInstanceMaterial();
}

void Geometry::finalize(Model* model)
{
    if (_geometry)
        _geometry->destroy();

    _geometry = Context::get().createGeometry(_type);

    _instance = _context->createGeometryInstance();
    _instance->setGeometry(_geometry);
    _bindInstanceMaterial();

    model->addGeometryInstance(_instance);
}

void Geometry::_setBuffer(const std::string& uniform,
                          ospray::Ref<ospray::Data> data)
{
    if (_buffer)
        _buffer->destroy();
    _buffer = _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_BYTE,
                                     data->numBytes);
    memcpy(_buffer->map(), data->data, data->numBytes);
    _buffer->unmap();
    _geometry[uniform]->setBuffer(_buffer);
}

void Geometry::_bindInstanceMaterial()
{
    if (!_instance)
        return;

    const Material* material = static_cast<const Material*>(_material.ptr);
    if (material)
    {
        _instance->setMaterialCount(1);
        _instance->setMaterial(0, material->optixMaterial);
    }
}
}
}
