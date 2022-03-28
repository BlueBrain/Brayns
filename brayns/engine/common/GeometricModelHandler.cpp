#include "GeometricModelHandler.h"

namespace brayns
{
OSPGeometricModel GeometricModelHandler::create()
{
    return ospNewGeometricModel();
}

void GeometricModelHandler::destory(OSPGeometricModel &model)
{
    if(model)
    {
        ospRelease(model);
        model = nullptr;
    }
}

void GeometricModelHandler::setMaterial(OSPGeometricModel model, Material &material)
{
    auto handle = material.handle();
    ospSetParam(model, "material", OSPDataType::OSP_MATERIAL, &handle);
}

void GeometricModelHandler::setColor(OSPGeometricModel model, const Vector3f &color)
{
    setColor(model, Vector4f(color, 1.f));
}

void GeometricModelHandler::setColor(OSPGeometricModel model, const Vector4f &color)
{
    ospSetParam(model, "color", OSPDataType::OSP_VEC4F, &color);
    ospRemoveParam(model, "index");
}

void GeometricModelHandler::setColors(OSPGeometricModel model, OSPBuffer &colors)
{
    ospSetParam(model, "color", OSPDataType::OSP_DATA, &colors.handle);
    ospRemoveParam(model, "index");
}

void GeometricModelHandler::setColorMap(OSPGeometricModel model, OSPBuffer &colors, OSPBuffer &indices)
{
    ospSetParam(model, "color", OSPDataType::OSP_DATA, &colors.handle);
    ospSetParam(model, "index", OSPDataType::OSP_DATA, &indices.handle);
}

void GeometricModelHandler::commitModel(OSPGeometricModel model)
{
    ospCommit(model);
}

void GeometricModelHandler::addToGeometryGroup(OSPGeometricModel model, Model &group)
{
    auto& groupObj = group.getGroup();
    groupObj.addGeometricModel(model);
}

void GeometricModelHandler::removeFromGeometryGroup(OSPGeometricModel model, Model &group)
{
    auto& groupObj = group.getGroup();
    groupObj.removeGeometricModel(model);
}

void GeometricModelHandler::addToClippingGroup(OSPGeometricModel model, Model &group)
{
    auto &groupObj = group.getGroup();
    groupObj.addClippingModel(model);
}

void GeometricModelHandler::removeFromClippingGroup(OSPGeometricModel model, Model &group)
{
    auto &groupObj = group.getGroup();
    groupObj.removeClippingModel(model);
}
}
