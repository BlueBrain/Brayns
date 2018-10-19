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

#pragma once

#include <ospray/SDK/api/Device.h>
#include <ospray/SDK/common/Managed.h>
#include <ospray/version.h>

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

namespace bbp
{
namespace optix
{
struct Device : public ospray::api::Device
{
    Device();
    ~Device();

    // ManagedObject Implementation /////////////////////////////////////////

    void commit() override;

    // Device Implementation ////////////////////////////////////////////////

    /*! create a new frame buffer */
    OSPFrameBuffer frameBufferCreate(const ospray::vec2i& size,
                                     const OSPFrameBufferFormat mode,
                                     const ospray::uint32 channels) override;

    /*! create a new transfer function object (out of list of
      registered transfer function types) */
    OSPTransferFunction newTransferFunction(const char* type) override;

    /*! create a new Light by type */
    OSPLight newLight(const char* type) override;

    /*! map frame buffer */
    const void* frameBufferMap(OSPFrameBuffer fb,
                               OSPFrameBufferChannel channel) override;

    /*! unmap previously mapped frame buffer */
    void frameBufferUnmap(const void* mapped, OSPFrameBuffer fb) override;

    /*! set a frame buffer's pixel op object */
    void setPixelOp(OSPFrameBuffer _fb, OSPPixelOp _op) override;

    /*! create a new pixelOp object (out of list of registered pixelOps) */
    OSPPixelOp newPixelOp(const char* type) override;

    /*! clear the specified channel(s) of the frame buffer specified in
        'whichChannels'

      if whichChannel&OSP_FB_COLOR!=0, clear the color buffer to
      '0,0,0,0'.

      if whichChannel&OSP_FB_DEPTH!=0, clear the depth buffer to
      +inf.

      if whichChannel&OSP_FB_ACCUM!=0, clear the accum buffer to 0,0,0,0,
      and reset accumID.
    */
    void frameBufferClear(OSPFrameBuffer _fb,
                          const ospray::uint32 fbChannelFlags) override;

    /*! create a new model */
    OSPModel newModel() override;

    /*! commit the given object's outstanding changes */
    void commit(OSPObject object) override;

    /*! add a new geometry to a model */
    void addGeometry(OSPModel _model, OSPGeometry _geometry) override;

    /*! remove an existing geometry from a model */
    void removeGeometry(OSPModel _model, OSPGeometry _geometry) override;

    /*! add a new volume to a model */
    void addVolume(OSPModel _model, OSPVolume _volume) override;

    /*! remove an existing volume from a model */
    void removeVolume(OSPModel _model, OSPVolume _volume) override;

    /*! create a new data buffer */
    OSPData newData(size_t nitems, OSPDataType format, const void* init,
                    int flags) override;

    /*! Copy data into the given volume. */
    int setRegion(OSPVolume object, const void* source,
                  const ospray::vec3i& index,
                  const ospray::vec3i& count) override;

    /*! assign (named) string parameter to an object */
    void setString(OSPObject object, const char* bufName,
                   const char* s) override;

    /*! assign (named) float parameter to an object */
    void setBool(OSPObject object, const char *bufName, const bool f) override;

    /*! assign (named) data item as a parameter to an object */
    void setObject(OSPObject target, const char* bufName,
                   OSPObject value) override;

    /*! assign (named) float parameter to an object */
    void setFloat(OSPObject object, const char* bufName,
                  const float f) override;

    /*! assign (named) vec2f parameter to an object */
    void setVec2f(OSPObject object, const char* bufName,
                  const ospray::vec2f& v) override;

    /*! assign (named) vec3f parameter to an object */
    void setVec3f(OSPObject object, const char* bufName,
                  const ospray::vec3f& v) override;

    /*! assign (named) vec4f parameter to an object */
    void setVec4f(OSPObject object, const char* bufName,
                  const ospray::vec4f& v) override;

    /*! assign (named) int parameter to an object */
    void setInt(OSPObject object, const char* bufName, const int f) override;

    /*! assign (named) vec2i parameter to an object */
    void setVec2i(OSPObject object, const char* bufName,
                  const ospray::vec2i& v) override;

    /*! assign (named) vec3i parameter to an object */
    void setVec3i(OSPObject object, const char* bufName,
                  const ospray::vec3i& v) override;

    /*! add untyped void pointer to object - this will *ONLY* work in local
        rendering!  */
    void setVoidPtr(OSPObject object, const char* bufName, void* v) override;

    void removeParam(OSPObject object, const char* name) override;

    /*! create a new renderer object (out of list of registered renderers) */
    OSPRenderer newRenderer(const char* type) override;

    /*! create a new geometry object (out of list of registered geometries) */
    OSPGeometry newGeometry(const char* type) override;

    /*! have given renderer create a new material */
    OSPMaterial newMaterial(OSPRenderer _renderer, const char* type) override;

    /*! have given renderer create a new material */
    OSPMaterial newMaterial(const char* renderer_type,
                            const char* material_type) override;

    /*! create a new camera object (out of list of registered cameras) */
    OSPCamera newCamera(const char* type) override;

    /*! create a new volume object (out of list of registered volumes) */
    OSPVolume newVolume(const char* type) override;

    /*! call a renderer to render a frame buffer */
    float renderFrame(OSPFrameBuffer _sc, OSPRenderer _renderer,
                      const ospray::uint32 fbChannelFlags) override;

    /*! load module */
    int loadModule(const char* name) override;

    //! release (i.e., reduce refcount of) given object
    /*! note that all objects in ospray are refcounted, so one cannot
      explicitly "delete" any object. instead, each object is created
      with a refcount of 1, and this refcount will be
      increased/decreased every time another object refers to this
      object resp releases its hold on it override; if the refcount is 0 the
      object will automatically get deleted. For example, you can
      create a new material, assign it to a geometry, and immediately
      after this assignation release its refcount override; the material will
      stay 'alive' as long as the given geometry requires it. */
    void release(OSPObject _obj) override;

    //! assign given material to given geometry
    void setMaterial(OSPGeometry _geom, OSPMaterial _mat) override;

    /*! create a new Texture2D object */
    OSPTexture newTexture(const char* type) override;

    OSPPickResult pick(OSPRenderer renderer,
                       const ospray::vec2f& screenPos) override;

private:
    void initializeDevice();

    ospray::ObjectHandle allocateHandle() const;

    bool _initialized{false};

    ::optix::Context _context;
};
}
}
