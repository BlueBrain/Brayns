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

#include <ospray/SDK/common/Data.h>

#include "TriangleMesh.h"

#include "../Model.h"
#include "../render/Material.h"
#include "Context.h"

namespace bbp
{
namespace optix
{
TriangleMesh::~TriangleMesh()
{
    if (_verticesBuffer)
        _verticesBuffer->destroy();
    if (_indicesBuffer)
        _indicesBuffer->destroy();
    if (_normalsBuffer)
        _normalsBuffer->destroy();
    if (_textureCoordsBuffer)
        _textureCoordsBuffer->destroy();
}

std::string TriangleMesh::toString() const
{
    return "optix::TriangleMesh";
}

void TriangleMesh::finalize(Model* optixModel)
{
    ospray::Ref<ospray::Data> vertexData =
        getParamData("vertex", getParamData("position"));
    ospray::Ref<ospray::Data> indexData =
        getParamData("index", getParamData("triangle"));

    if (!vertexData.ptr || !indexData.ptr)
        throw std::runtime_error(
            "#optix:geometry/trianglemesh: "
            "no 'position' or 'triangle' data specified");

    if (indexData->numItems == 0)
    {
        _verticesBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);
        _context["vertices_buffer"]->setBuffer(_verticesBuffer);
        _indicesBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT3, 0);
        _context["indices_buffer"]->setBuffer(_indicesBuffer);
        _normalsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);
        _context["normal_buffer"]->setBuffer(_normalsBuffer);
        _textureCoordsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, 0);
        _context["texcoord_buffer"]->setBuffer(_textureCoordsBuffer);
        return;
    }

    Geometry::finalize(optixModel);

    _verticesBuffer = _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
                                             vertexData->numItems);
    memcpy(_verticesBuffer->map(), vertexData->data, vertexData->numBytes);
    _verticesBuffer->unmap();

    _indicesBuffer = _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT3,
                                            indexData->numItems);
    memcpy(_indicesBuffer->map(), indexData->data, indexData->numBytes);
    _indicesBuffer->unmap();

    ospray::Ref<ospray::Data> normalData =
        getParamData("vertex.normal", getParamData("normal"));
    if (normalData && normalData->numItems > 0)
    {
        _normalsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
                                   normalData->numItems);
        memcpy(_normalsBuffer->map(), normalData->data, normalData->numBytes);
        _normalsBuffer->unmap();
    }
    else
        _normalsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);

    ospray::Ref<ospray::Data> texcoordData =
        getParamData("vertex.texcoord", getParamData("texcoord"));
    if (texcoordData && texcoordData->numItems > 0)
    {
        _textureCoordsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2,
                                   texcoordData->numItems);
        memcpy(_textureCoordsBuffer->map(), texcoordData->data,
               texcoordData->numBytes);
        _textureCoordsBuffer->unmap();
    }
    else
        _textureCoordsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, 0);

    _geometry->setPrimitiveCount(indexData->numItems);
    _geometry["vertices_buffer"]->setBuffer(_verticesBuffer);
    _geometry["indices_buffer"]->setBuffer(_indicesBuffer);
    _geometry["normal_buffer"]->setBuffer(_normalsBuffer);
    _geometry["texcoord_buffer"]->setBuffer(_textureCoordsBuffer);
}
}
}
