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

#pragma once

#include <ospray/SDK/common/Managed.h>
#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/common/OSPCommon.h>

#include <optixu/optixpp_namespace.h>

namespace bbp
{
namespace optix
{
struct Model;

class Geometry : public ospray::ManagedObject
{
public:
    enum Type
    {
        Cones,
        Cylinders,
        Spheres,
        TriangleMesh,
        Instance,
        SIZE
    };

    explicit Geometry(Type type);
    virtual ~Geometry();

    //! set given geometry's material.
    /*! all material assignations should go through this function; the
        'material' field itself is private). This allows the
        respective geometry's derived instance to always properly set
        the material field of the ISCP-equivalent whenever the
        c++-side's material gets changed */
    virtual void setMaterial(ospray::Material* mat);

    //! \brief common function to help printf-debugging
    virtual std::string toString() const = 0;

    /*! \brief integrates this geometry's primitives into the respective
        model's acceleration structure */
    virtual void finalize(Model* model);

protected:
    void _setBuffer(const std::string& uniform, ospray::Ref<ospray::Data> data);
    const Type _type;
    ::optix::Geometry _geometry;
    ::optix::Context _context;
    ::optix::Buffer _buffer;

    //! material associated to this geometry
    /*! this field is private to make sure it is only set through
        'setMaterial' (see comment there) */
    ospray::Ref<ospray::Material> _material;
};
}
}
