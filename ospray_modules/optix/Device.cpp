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

#include "Device.h"

#include <ospray/SDK/common/Data.h>
#include <ospray/SDK/texture/Texture2D.h>

#include "Context.h"
#include "Model.h"
#include "camera/Camera.h"
#include "framebuffer/FrameBuffer.h"
#include "geom/Spheres.h"
#include "lights/Light.h"
#include "render/Material.h"
#include "render/Renderer.h"

namespace bbp
{
namespace optix
{
Device::Device()
{
}

Device::~Device()
{
    Context::destroy();
}

void Device::initializeDevice()
{
    if (_initialized)
        return;

    ospray::api::Device::commit();

    _context = Context::get().getOptixContext();

    _initialized = true;
}

void Device::commit()
{
    if (!_initialized)
        initializeDevice();
}

OSPFrameBuffer Device::frameBufferCreate(const ospray::vec2i &size,
                                         const OSPFrameBufferFormat mode,
                                         const ospray::uint32 channels)
{
    FrameBuffer::ColorBufferFormat colorBufferFormat = mode;
    bool hasDepthBuffer = (channels & OSP_FB_DEPTH) != 0;
    bool hasAccumBuffer = (channels & OSP_FB_ACCUM) != 0;
    bool hasVarianceBuffer = (channels & OSP_FB_VARIANCE) != 0;

    FrameBuffer *fb = new FrameBuffer(size, colorBufferFormat, hasDepthBuffer,
                                      hasAccumBuffer, hasVarianceBuffer);
    fb->refInc();
    return (OSPFrameBuffer)fb;
}

const void *Device::frameBufferMap(OSPFrameBuffer _fb,
                                   OSPFrameBufferChannel channel)
{
    FrameBuffer *fb = (FrameBuffer *)_fb;
    switch (channel)
    {
    case OSP_FB_COLOR:
        return fb->mapColorBuffer();
    case OSP_FB_DEPTH:
        return fb->mapDepthBuffer();
    default:
        return nullptr;
    }
}

void Device::frameBufferUnmap(const void *mapped, OSPFrameBuffer _fb)
{
    Assert2(_fb != nullptr, "invalid framebuffer");
    FrameBuffer *fb = (FrameBuffer *)_fb;
    fb->unmap(mapped);
}

OSPModel Device::newModel()
{
    Model *model = new Model();
    model->refInc();
    return (OSPModel)model;
}

void Device::commit(OSPObject _object)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert2(object, "null object in LocalDevice::commit()");
    object->commit();
}

void Device::addGeometry(OSPModel _model, OSPGeometry _geometry)
{
    Model *model = (Model *)_model;
    Assert2(model, "null model in Device::addModel()");

    Geometry *geometry = (Geometry *)_geometry;
    Assert2(geometry, "null geometry in Device::addGeometry()");

    model->geometry.push_back(geometry);
}

void Device::addVolume(OSPModel /*_model*/, OSPVolume /*_volume*/)
{
}

OSPData Device::newData(size_t nitems, OSPDataType format, const void *init,
                        int flags)
{
    ospray::Data *data = new ospray::Data(nitems, format, init, flags);
    data->refInc();
    return (OSPData)data;
}

void Device::setVoidPtr(OSPObject _object, const char *bufName, void *v)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");
    object->setParam(bufName, v);
}

void Device::removeParam(OSPObject _object, const char *name)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(name != nullptr && "invalid identifier for object parameter");
    object->removeParam(name);
}

int Device::setRegion(OSPVolume /*_volume*/, const void * /*source*/,
                      const ospray::vec3i & /*index*/,
                      const ospray::vec3i & /*count*/)
{
    return -1;
}

void Device::setString(OSPObject _object, const char *bufName, const char *s)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");
    object->setParam<std::string>(bufName, s);
}

int Device::loadModule(const char *name)
{
    // HACK: would need ospFinish() to destroy this device which would destroy
    // the optix context
    if (std::string(name) == "exit")
        Context::destroy();
    return 0;
}

void Device::setFloat(OSPObject _object, const char *bufName, const float f)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, f);
}

void Device::setInt(OSPObject _object, const char *bufName, const int i)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, i);
}

void Device::setVec2f(OSPObject _object, const char *bufName,
                      const ospray::vec2f &v)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, v);
}

void Device::setVec3f(OSPObject _object, const char *bufName,
                      const ospray::vec3f &v)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, v);
}

void Device::setVec4f(OSPObject _object, const char *bufName,
                      const ospray::vec4f &v)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, v);
}

void Device::setVec2i(OSPObject _object, const char *bufName,
                      const ospray::vec2i &v)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, v);
}

void Device::setVec3i(OSPObject _object, const char *bufName,
                      const ospray::vec3i &v)
{
    ospray::ManagedObject *object = (ospray::ManagedObject *)_object;
    Assert(object != nullptr && "invalid object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    object->setParam(bufName, v);
}

void Device::setObject(OSPObject _target, const char *bufName, OSPObject _value)
{
    ospray::ManagedObject *target = (ospray::ManagedObject *)_target;
    ospray::ManagedObject *value = (ospray::ManagedObject *)_value;

    Assert(target != nullptr && "invalid target object handle");
    Assert(bufName != nullptr && "invalid identifier for object parameter");

    target->setParam(bufName, value);
}

OSPPixelOp Device::newPixelOp(const char * /*type*/)
{
    return nullptr;
}

void Device::setPixelOp(OSPFrameBuffer /*_fb*/, OSPPixelOp /*_op*/)
{
}

OSPRenderer Device::newRenderer(const char *type)
{
    Assert(type != nullptr && "invalid render type identifier");
    Renderer *renderer = (Renderer *)Renderer::createInstance(type);
    if (!renderer)
    {
        if (debugMode)
        {
            throw std::runtime_error("unknown renderer type '" +
                                     std::string(type) + "'");
        }
        else
            return nullptr;
    }
    renderer->refInc();
    return (OSPRenderer)renderer;
}

OSPCamera Device::newCamera(const char *type)
{
    Assert(type != nullptr && "invalid camera type identifier");
    Camera *camera = (Camera *)Camera::createInstance(type);
    if (!camera)
    {
        if (debugMode)
        {
            throw std::runtime_error("unknown camera type '" +
                                     std::string(type) + "'");
        }
        else
            return nullptr;
    }

    camera->refInc();
    return (OSPCamera)camera;
}

OSPVolume Device::newVolume(const char * /*type*/)
{
    return nullptr;
}

OSPGeometry Device::newGeometry(const char *type)
{
    Assert(type != nullptr && "invalid render type identifier");
    Geometry *geometry = Geometry::createInstance(type);
    if (!geometry)
        return nullptr;
    geometry->refInc();
    return (OSPGeometry)geometry;
}

OSPMaterial Device::newMaterial(OSPRenderer _renderer, const char *type)
{
    UNUSED(_renderer);
    Assert2(type != nullptr, "invalid material type identifier");

    Material *material = (Material *)Material::createMaterial(type);
    if (!material)
        return nullptr;
    material->refInc();
    return (OSPMaterial)material;
}

#if ((OSPRAY_VERSION_MAJOR == 1) && (OSPRAY_VERSION_MINOR > 4))
OSPMaterial Device::newMaterial(const char *renderer_type,
                                const char *material_type)
{
    auto renderer = newRenderer(renderer_type);
    auto material = newMaterial(renderer, material_type);
    release(renderer);
    return material;
}
#endif

OSPTransferFunction Device::newTransferFunction(const char * /*type*/)
{
    return nullptr;
}

OSPLight Device::newLight(OSPRenderer _renderer, const char *type)
{
    UNUSED(_renderer);
    ospray::Light *light = ospray::Light::createLight(type);
    if (!light)
        return nullptr;
    light->refInc();
    return (OSPLight)light;
}

#if ((OSPRAY_VERSION_MAJOR == 1) && (OSPRAY_VERSION_MINOR > 4))
OSPLight Device::newLight(const char *renderer_type, const char *light_type)
{
    auto renderer = newRenderer(renderer_type);
    auto light = newLight(renderer, light_type);
    release(renderer);
    return light;
}
#endif

void Device::frameBufferClear(OSPFrameBuffer _fb,
                              const ospray::uint32 fbChannelFlags)
{
    FrameBuffer *fb = (FrameBuffer *)_fb;
    fb->clear(fbChannelFlags);
}

void Device::removeGeometry(OSPModel _model, OSPGeometry _geometry)
{
    Model *model = (Model *)_model;
    Assert2(model, "null model in LocalDevice::removeGeometry");

    Geometry *geometry = (Geometry *)_geometry;
    Assert2(geometry, "null geometry in LocalDevice::removeGeometry");

    auto it = std::find_if(model->geometry.begin(), model->geometry.end(),
                           [&](const ospray::Ref<Geometry> &g) {
                               return geometry == &*g;
                           });

    if (it != model->geometry.end())
    {
        model->geometry.erase(it);
    }
}

void Device::removeVolume(OSPModel /*_model*/, OSPVolume /*_volume*/)
{
}

float Device::renderFrame(OSPFrameBuffer _fb, OSPRenderer _renderer,
                          const ospray::uint32 fbChannelFlags)
{
    FrameBuffer *fb = (FrameBuffer *)_fb;
    Renderer *renderer = (Renderer *)_renderer;

    Assert(fb != nullptr && "invalid frame buffer handle");
    Assert(renderer != nullptr && "invalid renderer handle");

    try
    {
        return renderer->renderFrame(fb, fbChannelFlags);
    }
    catch (const std::exception &e)
    {
        ospray::postStatusMsg()
            << "================================================\n"
            << "# >>> ospray fatal error <<< \n"
            << std::string(e.what()) + '\n'
            << "================================================";
        exit(1);
    }
}

void Device::release(OSPObject _obj)
{
    if (!_obj)
        return;
    ospray::ManagedObject *obj = (ospray::ManagedObject *)_obj;

    // TODO: would need to check for refcount == 1, but private. So better move
    // this to our own Texture2D class' dtor.
    if (auto *tx = dynamic_cast<ospray::Texture2D *>(obj))
        Context::get().deleteTexture(tx);

    obj->refDec();
}

void Device::setMaterial(OSPGeometry _geometry, OSPMaterial _material)
{
    Geometry *geometry = (Geometry *)_geometry;
    Material *material = (Material *)_material;
    assert(geometry);
    assert(material);
    geometry->setMaterial(material);
}

OSPTexture2D Device::newTexture2D(const ospray::vec2i &size,
                                  const OSPTextureFormat type, void *data,
                                  const ospray::uint32 flags)
{
    Assert(size.x > 0 &&
           "Width must be greater than 0 in optix::Device::newTexture2D");
    Assert(size.y > 0 &&
           "Height must be greater than 0 in optix::Device::newTexture2D");

    auto *tx = new ospray::Texture2D;
    tx->refInc();

    // from ospray::Texture2D::createTexture(), w/o the ispc part
    tx->size = size;
    tx->type = type;
    tx->flags = flags;
    tx->managedObjectType = OSP_TEXTURE;

    assert(data);

    if (flags & OSP_TEXTURE_SHARED_BUFFER)
        tx->data = data;
    else
    {
        const size_t bytes = ospray::sizeOf(type) * size.x * size.y;
        tx->data = bytes ? new unsigned char[bytes] : NULL;
        memcpy(tx->data, data, bytes);
    }

    Context::get().createTexture(tx);

    return (OSPTexture2D)tx;
}

OSPPickResult Device::pick(OSPRenderer /*renderer*/,
                           const ospray::vec2f & /*screenPos*/)
{
    return OSPPickResult();
}

ospray::ObjectHandle Device::allocateHandle() const
{
    return ospray::ObjectHandle();
}

OSP_REGISTER_DEVICE(Device, optix);
}
}

extern "C" OSPRAY_DLLEXPORT void ospray_init_module_optix()
{
    std::cout << "#ospray: initializing optix plugin" << std::endl;
}
