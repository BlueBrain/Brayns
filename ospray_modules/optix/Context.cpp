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

#include "Context.h"

#include <ospray/SDK/common/OSPCommon.h>
#include <ospray/SDK/texture/Texture2D.h>

#include <ospray_module_optix_generated_Cones.cu.ptx.h>
#include <ospray_module_optix_generated_Cylinders.cu.ptx.h>
#include <ospray_module_optix_generated_Phong.cu.ptx.h>
#include <ospray_module_optix_generated_Spheres.cu.ptx.h>
#include <ospray_module_optix_generated_TrianglesMesh.cu.ptx.h>

#include "CommonStructs.h"
#include "lights/Light.h"

// For rtDevice*() function error checking. No OptiX context present at that
// time.
#define RT_CHECK_ERROR_NO_CONTEXT(func)                            \
    do                                                             \
    {                                                              \
        RTresult code = func;                                      \
        if (code != RT_SUCCESS)                                    \
            throw std::runtime_error("Optix error in function '" + \
                                     std::string(#func) + "'");    \
    } while (0)

namespace bbp
{
namespace optix
{
char const* const CUDA_PHONG = ospray_module_optix_generated_Phong_cu_ptx;
char const* const CUDA_SPHERES = ospray_module_optix_generated_Spheres_cu_ptx;
char const* const CUDA_TRIANGLES_MESH =
    ospray_module_optix_generated_TrianglesMesh_cu_ptx;
char const* const CUDA_CONES = ospray_module_optix_generated_Cones_cu_ptx;
char const* const CUDA_CYLINDERS =
    ospray_module_optix_generated_Cylinders_cu_ptx;

const std::string CUDA_FUNCTION_BOUNDS = "bounds";
const std::string CUDA_FUNCTION_INTERSECTION = "intersect";

std::unique_ptr<Context> Context::_context;

Context::Context()
{
    _printSystemInformation();
    _initialize();
}

Context::~Context()
{
    _phong_ah->destroy();
    _phong_ch->destroy();
    _phong_ch_textured->destroy();

    for (auto& i : _bounds)
        if (i)
            i->destroy();
    for (auto& i : _intersects)
        if (i)
            i->destroy();

    _lightBuffer->destroy();
    _optixContext->destroy();
}

Context& Context::get()
{
    if (!_context)
        _context.reset(new Context);
    return *_context;
}

::optix::Context Context::getOptixContext()
{
    return _optixContext;
}

::optix::Material Context::createMaterial(const bool textured)
{
    auto optixMaterial = _optixContext->createMaterial();
    optixMaterial->setClosestHitProgram(0, textured ? _phong_ch_textured
                                                    : _phong_ch);
    optixMaterial->setAnyHitProgram(1, _phong_ah);
    return optixMaterial;
}

void Context::createTexture(ospray::Texture2D* tx)
{
    const uint16_t nx = tx->size.x;
    const uint16_t ny = tx->size.y;
    uint16_t channels = 3;
    switch (tx->type)
    {
    case OSP_TEXTURE_RGBA8:
    case OSP_TEXTURE_SRGBA:
    case OSP_TEXTURE_RGBA32F:
        channels = 4;
        break;
    default:
        channels = 3;
    }
    const uint16_t optixChannels = 4;

    // Create texture sampler
    ::optix::TextureSampler sampler = _optixContext->createTextureSampler();
    sampler->setWrapMode(0, RT_WRAP_REPEAT);
    sampler->setWrapMode(1, RT_WRAP_REPEAT);
    sampler->setWrapMode(2, RT_WRAP_REPEAT);
    sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
    sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    sampler->setMaxAnisotropy(1.0f);
    sampler->setMipLevelCount(1u);
    sampler->setArraySize(1u);

    // Create buffer and populate with texture data
    ::optix::Buffer buffer =
        _optixContext->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, nx, ny);
    float* buffer_data = static_cast<float*>(buffer->map());

    size_t idx_src = 0;
    size_t idx_dst = 0;
    for (uint16_t y = 0; y < ny; ++y)
        for (uint16_t x = 0; x < nx; ++x)
        {
            switch (tx->type)
            {
            case OSP_TEXTURE_RGBA32F:
            case OSP_TEXTURE_RGB32F:
            case OSP_TEXTURE_R32F:
            {
                auto data = (float*)tx->data;
                buffer_data[idx_dst + 0] = data[idx_src + 2];
                buffer_data[idx_dst + 1] = data[idx_src + 1];
                buffer_data[idx_dst + 2] = data[idx_src + 0];
                buffer_data[idx_dst + 3] =
                    (channels == optixChannels) ? data[idx_src + 4] : 1.f;
            }
            break;
            default:
            {
                auto data = (unsigned char*)tx->data;
                buffer_data[idx_dst + 0] = float(data[idx_src + 2]) / 255.f;
                buffer_data[idx_dst + 1] = float(data[idx_src + 1]) / 255.f;
                buffer_data[idx_dst + 2] = float(data[idx_src + 0]) / 255.f;
                buffer_data[idx_dst + 3] =
                    (channels == optixChannels)
                        ? float(data[idx_src + 4]) / 255.f
                        : 1.f;
            }
            }

            idx_dst += 4;
            idx_src += (channels == optixChannels) ? 4 : 3;
        }

    buffer->unmap();

    // Assign buffer to sampler
    sampler->setBuffer(buffer);
    sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR,
                               RT_FILTER_NONE);
    _optixTextureSamplers[tx] = sampler;
}

void Context::deleteTexture(ospray::Texture2D* tx)
{
    _optixTextureSamplers.erase(tx);
}

::optix::TextureSampler Context::getTextureSampler(ospray::Texture2D* tx)
{
    return _optixTextureSamplers[tx];
}

void Context::updateLights(ospray::Data* lightData)
{
    if (_lightBuffer)
        _lightBuffer->destroy();
    _lightBuffer = nullptr;

    if (lightData)
        _lightData = lightData;

    if (_lightData)
    {
        if (!_lightBuffer)
        {
            _lightBuffer = _optixContext->createBuffer(RT_BUFFER_INPUT);
            _lightBuffer->setFormat(RT_FORMAT_USER);
            _lightBuffer->setElementSize(sizeof(BasicLight));
            _lightBuffer->setSize(_lightData->size());
            _optixContext["lights"]->set(_lightBuffer);
        }

        std::vector<BasicLight> optixLights;

        for (uint32_t i = 0; i < _lightData->size(); i++)
        {
            auto osprayLight = ((ospray::Light**)_lightData->data)[i];
            auto light = dynamic_cast<const Light*>(osprayLight);
            if (light)
            {
                optixLights.push_back(light->optixLight);
            }
        }

        if (_lightBuffer && !optixLights.empty())
        {
            memcpy(_lightBuffer->map(), &optixLights[0],
                   optixLights.size() * sizeof(BasicLight));
            _lightBuffer->unmap();
        }
    }
}

::optix::Geometry Context::createGeometry(const Geometry::Type type)
{
    ::optix::Geometry geometry = _optixContext->createGeometry();
    geometry->setBoundingBoxProgram(_bounds[type]);
    geometry->setIntersectionProgram(_intersects[type]);
    return geometry;
}

void Context::destroy()
{
    _context.reset();
}

void Context::_initialize()
{
    _optixContext = ::optix::Context::create();

    if (!_optixContext)
        throw(std::runtime_error("Failed to initialize OptiX"));

    _optixContext->setRayTypeCount(2);
    _optixContext->setEntryPointCount(1);
    _optixContext->setStackSize(4096);

    _phong_ch =
        _optixContext->createProgramFromPTXString(CUDA_PHONG,
                                                  "closest_hit_radiance");

    _phong_ch_textured = _optixContext->createProgramFromPTXString(
        CUDA_PHONG, "closest_hit_radiance_textured");

    _phong_ah =
        _optixContext->createProgramFromPTXString(CUDA_PHONG, "any_hit_shadow");

    _bounds[Geometry::Type::Cones] =
        _optixContext->createProgramFromPTXString(CUDA_CONES,
                                                  CUDA_FUNCTION_BOUNDS);
    _intersects[Geometry::Type::Cones] =
        _optixContext->createProgramFromPTXString(CUDA_CONES,
                                                  CUDA_FUNCTION_INTERSECTION);

    _bounds[Geometry::Type::Cylinders] =
        _optixContext->createProgramFromPTXString(CUDA_CYLINDERS,
                                                  CUDA_FUNCTION_BOUNDS);
    _intersects[Geometry::Type::Cylinders] =
        _optixContext->createProgramFromPTXString(CUDA_CYLINDERS,
                                                  CUDA_FUNCTION_INTERSECTION);

    _bounds[Geometry::Type::Spheres] =
        _optixContext->createProgramFromPTXString(CUDA_SPHERES,
                                                  CUDA_FUNCTION_BOUNDS);
    _intersects[Geometry::Type::Spheres] =
        _optixContext->createProgramFromPTXString(CUDA_SPHERES,
                                                  CUDA_FUNCTION_INTERSECTION);

    _bounds[Geometry::Type::TriangleMesh] =
        _optixContext->createProgramFromPTXString(CUDA_TRIANGLES_MESH,
                                                  CUDA_FUNCTION_BOUNDS);
    _intersects[Geometry::Type::TriangleMesh] =
        _optixContext->createProgramFromPTXString(CUDA_TRIANGLES_MESH,
                                                  CUDA_FUNCTION_INTERSECTION);
}

void Context::_printSystemInformation() const
{
    unsigned int optixVersion;
    RT_CHECK_ERROR_NO_CONTEXT(rtGetVersion(&optixVersion));

    unsigned int major = optixVersion / 1000; // Check major with old formula.
    unsigned int minor;
    unsigned int micro;
    if (3 < major) // New encoding since OptiX 4.0.0 to get two digits micro
                   // numbers?
    {
        major = optixVersion / 10000;
        minor = (optixVersion % 10000) / 100;
        micro = optixVersion % 100;
    }
    else // Old encoding with only one digit for the micro number.
    {
        minor = (optixVersion % 1000) / 10;
        micro = optixVersion % 10;
    }
    ospray::postStatusMsg() << "OptiX " << major << "." << minor << "."
                            << micro;

    unsigned int numberOfDevices = 0;
    RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetDeviceCount(&numberOfDevices));
    ospray::postStatusMsg() << "Number of Devices = " << numberOfDevices
                            << std::endl;

    for (unsigned int i = 0; i < numberOfDevices; ++i)
    {
        char name[256];
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i,
                                                       RT_DEVICE_ATTRIBUTE_NAME,
                                                       sizeof(name), name));
        ospray::postStatusMsg() << "Device " << i << ": " << name;

        int computeCapability[2] = {0, 0};
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY,
                                 sizeof(computeCapability),
                                 &computeCapability));
        ospray::postStatusMsg() << "  Compute Support: " << computeCapability[0]
                                << "." << computeCapability[1];

        RTsize totalMemory = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_TOTAL_MEMORY,
                                 sizeof(totalMemory), &totalMemory));
        ospray::postStatusMsg()
            << "  Total Memory: "
            << (unsigned long long)(totalMemory / 1024 / 1024) << " MB";

        int clockRate = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CLOCK_RATE,
                                 sizeof(clockRate), &clockRate));
        ospray::postStatusMsg() << "  Clock Rate: " << (clockRate / 1000)
                                << " MHz";

        int maxThreadsPerBlock = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK,
                                 sizeof(maxThreadsPerBlock),
                                 &maxThreadsPerBlock));
        ospray::postStatusMsg() << "  Max. Threads per Block: "
                                << maxThreadsPerBlock;

        int smCount = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,
                                 sizeof(smCount), &smCount));
        ospray::postStatusMsg() << "  Streaming Multiprocessor Count: "
                                << smCount;

        int executionTimeoutEnabled = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_EXECUTION_TIMEOUT_ENABLED,
            sizeof(executionTimeoutEnabled), &executionTimeoutEnabled));
        ospray::postStatusMsg() << "  Execution Timeout Enabled: "
                                << executionTimeoutEnabled;

        int maxHardwareTextureCount = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_MAX_HARDWARE_TEXTURE_COUNT,
            sizeof(maxHardwareTextureCount), &maxHardwareTextureCount));
        ospray::postStatusMsg() << "  Max. Hardware Texture Count: "
                                << maxHardwareTextureCount;

        int tccDriver = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_TCC_DRIVER,
                                 sizeof(tccDriver), &tccDriver));
        ospray::postStatusMsg() << "  TCC Driver enabled: " << tccDriver;

        int cudaDeviceOrdinal = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CUDA_DEVICE_ORDINAL,
                                 sizeof(cudaDeviceOrdinal),
                                 &cudaDeviceOrdinal));
        ospray::postStatusMsg()
            << "  CUDA Device Ordinal: " << cudaDeviceOrdinal << std::endl;
    }
}
}
}
