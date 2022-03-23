#pragma once

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/RenderableType.h>
#include <brayns/engine/common/DataHandler.h>

#include <ospray/ospray.h>

namespace brayns
{
struct GeometricModelHandler
{
    static OSPGeometricModel create();

    static void destory(OSPGeometricModel &model);

    template<typename T>
    static void setGeometry(OSPGeometricModel model, Geometry<T> &geometry)
    {
            auto geometryHandle = geometry.handle();
            ospSetParam(model, "geometry", OSPDataType::OSP_GEOMETRY, &geometryHandle);
    }

    static void setMaterial(OSPGeometricModel model, Material &material);

    static void setColor(OSPGeometricModel model, const Vector3f &color);

    static void setColor(OSPGeometricModel model, const Vector4f &color);

    static void setColors(OSPGeometricModel model, Buffer &colors);

    static void setColorMap(OSPGeometricModel model, Buffer &colors, Buffer &indices);

    static void commitModel(OSPGeometricModel model);

    static void addToGeometryGroup(OSPGeometricModel model, Model &group);

    static void removeFromGeometryGroup(OSPGeometricModel model, Model &group);

    static void addToClippingGroup(OSPGeometricModel model, Model &group);

    static void removeFromClippingGroup(OSPGeometricModel model, Model &group);
};
}
