/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "OptiXContext.h"

#include <engines/optix/braynsOptixEngine_generated_Cones.cu.ptx.h>
#include <engines/optix/braynsOptixEngine_generated_Cylinders.cu.ptx.h>
#include <engines/optix/braynsOptixEngine_generated_Spheres.cu.ptx.h>
#include <engines/optix/braynsOptixEngine_generated_TriangleMesh.cu.ptx.h>

#include <brayns/common/log.h>
#include <brayns/common/material/Texture2D.h>

namespace
{
const std::string DEFAULT_ACCELERATION_STRUCTURE = "Trbvh";
const std::string CUDA_SPHERES = braynsOptixEngine_generated_Spheres_cu_ptx;
const std::string CUDA_CYLINDERS = braynsOptixEngine_generated_Cylinders_cu_ptx;
const std::string CUDA_CONES = braynsOptixEngine_generated_Cones_cu_ptx;
const std::string CUDA_TRIANGLES_MESH =
    braynsOptixEngine_generated_TriangleMesh_cu_ptx;

const std::string CUDA_FUNC_BOUNDS = "bounds";
const std::string CUDA_FUNC_INTERSECTION = "intersect";
const std::string CUDA_FUNC_ROBUST_INTERSECTION = "robust_intersect";
const std::string CUDA_FUNC_EXCEPTION = "exception";

template <typename T>
T white();

template <>
uint8 white()
{
    return 255;
}

template <>
float white()
{
    return 1.f;
}

template <typename T>
void textureToOptix(T* ptr_dst, const brayns::Texture2D& texture,
                    const uint8_t face, const uint8_t mipLevel,
                    const bool hasAlpha)
{
    uint16_t width = texture.width;
    uint16_t height = texture.height;
    for (uint8_t i = 0; i < mipLevel; ++i)
    {
        width /= 2;
        height /= 2;
    }
    size_t idx_src = 0;
    size_t idx_dst = 0;
    const auto rawData = texture.getRawData<T>(face, mipLevel);
    for (uint16_t y = 0; y < height; ++y)
    {
        for (uint16_t x = 0; x < width; ++x)
        {
            ptr_dst[idx_dst] = rawData[idx_src];
            ptr_dst[idx_dst + 1u] = rawData[idx_src + 1u];
            ptr_dst[idx_dst + 2u] = rawData[idx_src + 2u];
            ptr_dst[idx_dst + 3u] =
                hasAlpha ? rawData[idx_src + 3u] : white<T>();
            idx_dst += 4u;
            idx_src += hasAlpha ? 4u : 3u;
        }
    }
}

RTwrapmode wrapModeToOptix(const brayns::TextureWrapMode mode)
{
    switch (mode)
    {
    case brayns::TextureWrapMode::clamp_to_border:
        return RT_WRAP_CLAMP_TO_BORDER;
    case brayns::TextureWrapMode::clamp_to_edge:
        return RT_WRAP_CLAMP_TO_EDGE;
    case brayns::TextureWrapMode::mirror:
        return RT_WRAP_MIRROR;
    case brayns::TextureWrapMode::repeat:
    default:
        return RT_WRAP_REPEAT;
    }
}

} // namespace

#define RT_CHECK_ERROR_NO_CONTEXT(func)                            \
    do                                                             \
    {                                                              \
        RTresult code = func;                                      \
        if (code != RT_SUCCESS)                                    \
            throw std::runtime_error("Optix error in function '" + \
                                     std::string(#func) + "'");    \
    } while (0)

constexpr size_t OPTIX_STACK_SIZE = 4096;
constexpr size_t OPTIX_RAY_TYPE_COUNT = 2;
constexpr size_t OPTIX_ENTRY_POINT_COUNT = 1;

namespace brayns
{
std::unique_ptr<OptiXContext> OptiXContext::_context;

OptiXContext::OptiXContext()
{
    _printSystemInformation();
    _initialize();
}

OptiXContext::~OptiXContext()
{
    if (_optixContext)
        _optixContext->destroy();
}

OptiXContext& OptiXContext::get()
{
    if (!_context)
        _context.reset(new OptiXContext);

    return *_context;
}

::optix::Material OptiXContext::createMaterial()
{
    return _optixContext->createMaterial();
}

void OptiXContext::addRenderer(const std::string& name,
                               OptiXShaderProgramPtr program)
{
    _rendererProgram[name] = program;
}

OptiXShaderProgramPtr OptiXContext::getRenderer(const std::string& name)
{
    auto it = _rendererProgram.find(name);
    if (it == _rendererProgram.end())
        throw std::runtime_error("Shader program not found for renderer '" +
                                 name + "'");
    return it->second;
}

void OptiXContext::addCamera(const std::string& name,
                             OptiXCameraProgramPtr program)
{
    _cameraProgram[name] = program;
}

OptiXCameraProgramPtr OptiXContext::getCamera(const std::string& name)
{
    auto it = _cameraProgram.find(name);
    if (it == _cameraProgram.end())
        throw std::runtime_error("Camera program not found for camera '" +
                                 name + "'");
    return it->second;
}

void OptiXContext::setCamera(const std::string& name)
{
    auto camera = getCamera(name);
    _optixContext->setRayGenerationProgram(0,
                                           camera->getRayGenerationProgram());
    _optixContext->setMissProgram(0, camera->getMissProgram());
    _optixContext->setExceptionProgram(0, camera->getExceptionProgram());
}

::optix::TextureSampler OptiXContext::createTextureSampler(Texture2DPtr texture)
{
    uint16_t nx = texture->width;
    uint16_t ny = texture->height;
    const uint16_t channels = texture->channels;
    const uint16_t optixChannels = 4;
    const bool hasAlpha = optixChannels == channels;

    const bool useFloat = texture->depth == 4;
    const bool useByte = texture->depth == 1;

    if (!useFloat && !useByte)
        throw std::runtime_error("Only byte or float textures are supported");

    const bool createMipmaps =
        texture->getMipLevels() == 1 && useByte && !texture->isCubeMap();
    uint16_t mipMapLevels = texture->getMipLevels();
    if (createMipmaps)
        mipMapLevels = texture->getPossibleMipMapsLevels();

    if (createMipmaps && !useByte)
        throw std::runtime_error(
            "Non 8-bits textures are not supported for automatic mipmaps "
            "generation");

    RTformat optixFormat =
        useByte ? RT_FORMAT_UNSIGNED_BYTE4 : RT_FORMAT_FLOAT4;

    // Create texture sampler
    ::optix::TextureSampler sampler = _optixContext->createTextureSampler();
    const auto wrapMode = wrapModeToOptix(texture->getWrapMode());
    sampler->setWrapMode(0, wrapMode);
    sampler->setWrapMode(1, wrapMode);
    sampler->setWrapMode(2, wrapMode);
    sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
    sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    sampler->setMaxAnisotropy(8.0f);

    // Create buffer and populate with texture data
    optix::Buffer buffer;
    if (texture->isCubeMap())
        buffer = _optixContext->createCubeBuffer(RT_BUFFER_INPUT, optixFormat,
                                                 nx, ny, mipMapLevels);
    else
        buffer =
            _optixContext->createMipmappedBuffer(RT_BUFFER_INPUT, optixFormat,
                                                 nx, ny, mipMapLevels);

    std::vector<void*> mipMapBuffers(mipMapLevels);
    for (uint8_t currentLevel = 0u; currentLevel < mipMapLevels; ++currentLevel)
        mipMapBuffers[currentLevel] = buffer->map(currentLevel);

    if (createMipmaps)
    {
        uint8_t* ptr_dst = (uint8_t*)mipMapBuffers[0];
        size_t idx_src = 0;
        size_t idx_dst = 0;
        const auto rawData = texture->getRawData<unsigned char>();
        for (uint16_t y = 0; y < ny; ++y)
        {
            for (uint16_t x = 0; x < nx; ++x)
            {
                ptr_dst[idx_dst] = rawData[idx_src];
                ptr_dst[idx_dst + 1u] = rawData[idx_src + 1u];
                ptr_dst[idx_dst + 2u] = rawData[idx_src + 2u];
                ptr_dst[idx_dst + 3u] = hasAlpha ? rawData[idx_src + 3u] : 255u;
                idx_dst += 4u;
                idx_src += hasAlpha ? 4u : 3u;
            }
        }
        ny /= 2u;
        nx /= 2u;

        for (uint8_t currentLevel = 1u; currentLevel < mipMapLevels;
             ++currentLevel)
        {
            ptr_dst = (uint8_t*)mipMapBuffers[currentLevel];
            uint8_t* ptr_src = (uint8_t*)mipMapBuffers[currentLevel - 1u];
            for (uint16_t y = 0u; y < ny; ++y)
            {
                for (uint16_t x = 0u; x < nx; ++x)
                {
                    ptr_dst[(y * nx + x) * 4u] =
                        (ptr_src[(y * 2u * nx + x) * 8u] +
                         ptr_src[((y * 2u * nx + x) * 2u + 1u) * 4u] +
                         ptr_src[((y * 2u + 1u) * nx + x) * 8u] +
                         ptr_src[(((y * 2u + 1u) * nx + x) * 2u + 1u) * 4u]) /
                        4.0f;
                    ptr_dst[(y * nx + x) * 4u + 1u] =
                        (ptr_src[(y * 2u * nx + x) * 8u + 1u] +
                         ptr_src[((y * 2u * nx + x) * 2u + 1u) * 4u + 1u] +
                         ptr_src[((y * 2u + 1u) * nx + x) * 8u + 1u] +
                         ptr_src[(((y * 2u + 1u) * nx + x) * 2u + 1u) * 4u +
                                 1u]) /
                        4.0f;
                    ptr_dst[(y * nx + x) * 4u + 2u] =
                        (ptr_src[(y * 2u * nx + x) * 8u + 2u] +
                         ptr_src[((y * 2u * nx + x) * 2u + 1u) * 4u + 2u] +
                         ptr_src[((y * 2u + 1u) * nx + x) * 8u + 2u] +
                         ptr_src[(((y * 2u + 1u) * nx + x) * 2u + 1u) * 4u +
                                 2u]) /
                        4.0f;
                    ptr_dst[(y * nx + x) * 4u + 3u] =
                        (ptr_src[(y * 2u * nx + x) * 8u + 3u] +
                         ptr_src[((y * 2u * nx + x) * 2u + 1u) * 4u + 3u] +
                         ptr_src[((y * 2u + 1u) * nx + x) * 8u + 3u] +
                         ptr_src[(((y * 2u + 1u) * nx + x) * 2u + 1u) * 4u +
                                 3u]) /
                        4.0f;

                    if (texture->isNormalMap())
                    {
                        glm::vec3 normalized = glm::normalize(glm::vec3(
                            2.0f * (float)ptr_dst[(y * nx + x) * 4u] / 255.0f -
                                1.0f,
                            2.0f * (float)ptr_dst[(y * nx + x) * 4u + 1u] /
                                    255.0f -
                                1.0f,
                            2.0f * (float)ptr_dst[(y * nx + x) * 4u + 2u] /
                                    255.0f -
                                1.0f));
                        ptr_dst[(y * nx + x) * 4u] =
                            255.0f * (0.5f * normalized.x + 0.5f);
                        ptr_dst[(y * nx + x) * 4u + 1u] =
                            255.0f * (0.5f * normalized.y + 0.5f);
                        ptr_dst[(y * nx + x) * 4u + 2u] =
                            255.0f * (0.5f * normalized.z + 0.5f);
                    }
                }
            }
            ny /= 2u;
            nx /= 2u;
        }
    }
    else
    {
        for (uint8_t face = 0; face < texture->getNumFaces(); ++face)
        {
            auto mipWidth = nx;
            auto mipHeight = ny;
            for (uint16_t mip = 0; mip < mipMapLevels; ++mip)
            {
                if (useByte)
                {
                    auto dst = (uint8_t*)mipMapBuffers[mip];
                    dst += face * mipWidth * mipHeight * 4;
                    textureToOptix<uint8_t>(dst, *texture, face, mip, hasAlpha);
                }
                else if (useFloat)
                {
                    auto dst = (float*)mipMapBuffers[mip];
                    dst += face * mipWidth * mipHeight * 4;
                    textureToOptix<float>(dst, *texture, face, mip, hasAlpha);
                }
                mipWidth /= 2;
                mipHeight /= 2;
            }
        }
    }

    for (uint8_t currentLevel = 0u; currentLevel < mipMapLevels; ++currentLevel)
        buffer->unmap(currentLevel);

    // Assign buffer to sampler
    sampler->setBuffer(buffer);
    sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR,
                               mipMapLevels > 1 ? RT_FILTER_LINEAR
                                                : RT_FILTER_NONE);
    sampler->validate();
    return sampler;
}

void OptiXContext::_initialize()
{
    BRAYNS_DEBUG << "Creating context..." << std::endl;
    _optixContext = ::optix::Context::create();

    if (!_optixContext)
        throw(std::runtime_error("Failed to initialize OptiX"));

    _optixContext->setRayTypeCount(OPTIX_RAY_TYPE_COUNT);
    _optixContext->setEntryPointCount(OPTIX_ENTRY_POINT_COUNT);
    _optixContext->setStackSize(OPTIX_STACK_SIZE);

    _bounds[OptixGeometryType::cone] =
        _optixContext->createProgramFromPTXString(CUDA_CONES, CUDA_FUNC_BOUNDS);
    _intersects[OptixGeometryType::cone] =
        _optixContext->createProgramFromPTXString(CUDA_CONES,
                                                  CUDA_FUNC_INTERSECTION);

    _bounds[OptixGeometryType::cylinder] =
        _optixContext->createProgramFromPTXString(CUDA_CYLINDERS,
                                                  CUDA_FUNC_BOUNDS);
    _intersects[OptixGeometryType::cylinder] =
        _optixContext->createProgramFromPTXString(CUDA_CYLINDERS,
                                                  CUDA_FUNC_INTERSECTION);

    _bounds[OptixGeometryType::sphere] =
        _optixContext->createProgramFromPTXString(CUDA_SPHERES,
                                                  CUDA_FUNC_BOUNDS);
    _intersects[OptixGeometryType::sphere] =
        _optixContext->createProgramFromPTXString(CUDA_SPHERES,
                                                  CUDA_FUNC_INTERSECTION);

    _bounds[OptixGeometryType::triangleMesh] =
        _optixContext->createProgramFromPTXString(CUDA_TRIANGLES_MESH,
                                                  CUDA_FUNC_BOUNDS);
    _intersects[OptixGeometryType::triangleMesh] =
        _optixContext->createProgramFromPTXString(CUDA_TRIANGLES_MESH,
                                                  CUDA_FUNC_INTERSECTION);
    BRAYNS_DEBUG << "Context created" << std::endl;
}

void OptiXContext::_printSystemInformation() const
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
    BRAYNS_INFO << "OptiX " << major << "." << minor << "." << micro
                << std::endl;

    unsigned int numberOfDevices = 0;
    RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetDeviceCount(&numberOfDevices));
    BRAYNS_INFO << "Number of Devices = " << numberOfDevices << std::endl;

    for (unsigned int i = 0; i < numberOfDevices; ++i)
    {
        char name[256];
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(i,
                                                       RT_DEVICE_ATTRIBUTE_NAME,
                                                       sizeof(name), name));
        BRAYNS_INFO << "Device " << i << ": " << name << std::endl;

        int computeCapability[2] = {0, 0};
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY,
                                 sizeof(computeCapability),
                                 &computeCapability));
        BRAYNS_INFO << "  Compute Support: " << computeCapability[0] << "."
                    << computeCapability[1] << std::endl;

        RTsize totalMemory = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_TOTAL_MEMORY,
                                 sizeof(totalMemory), &totalMemory));
        BRAYNS_INFO << "  Total Memory: "
                    << (unsigned long long)(totalMemory / 1024 / 1024) << " MB"
                    << std::endl;

        int clockRate = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CLOCK_RATE,
                                 sizeof(clockRate), &clockRate));
        BRAYNS_INFO << "  Clock Rate: " << (clockRate / 1000) << " MHz"
                    << std::endl;

        int maxThreadsPerBlock = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK,
                                 sizeof(maxThreadsPerBlock),
                                 &maxThreadsPerBlock));
        BRAYNS_INFO << "  Max. Threads per Block: " << maxThreadsPerBlock
                    << std::endl;

        int smCount = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,
                                 sizeof(smCount), &smCount));
        BRAYNS_INFO << "  Streaming Multiprocessor Count: " << smCount
                    << std::endl;

        int executionTimeoutEnabled = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_EXECUTION_TIMEOUT_ENABLED,
            sizeof(executionTimeoutEnabled), &executionTimeoutEnabled));
        BRAYNS_INFO << "  Execution Timeout Enabled: "
                    << executionTimeoutEnabled << std::endl;

        int maxHardwareTextureCount = 0;
        RT_CHECK_ERROR_NO_CONTEXT(rtDeviceGetAttribute(
            i, RT_DEVICE_ATTRIBUTE_MAX_HARDWARE_TEXTURE_COUNT,
            sizeof(maxHardwareTextureCount), &maxHardwareTextureCount));
        BRAYNS_INFO << "  Max. Hardware Texture Count: "
                    << maxHardwareTextureCount << std::endl;

        int tccDriver = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_TCC_DRIVER,
                                 sizeof(tccDriver), &tccDriver));
        BRAYNS_INFO << "  TCC Driver enabled: " << tccDriver << std::endl;

        int cudaDeviceOrdinal = 0;
        RT_CHECK_ERROR_NO_CONTEXT(
            rtDeviceGetAttribute(i, RT_DEVICE_ATTRIBUTE_CUDA_DEVICE_ORDINAL,
                                 sizeof(cudaDeviceOrdinal),
                                 &cudaDeviceOrdinal));
        BRAYNS_INFO << "  CUDA Device Ordinal: " << cudaDeviceOrdinal
                    << std::endl;
    }
}

::optix::Geometry OptiXContext::createGeometry(const OptixGeometryType type)
{
    ::optix::Geometry geometry = _optixContext->createGeometry();
    geometry->setBoundingBoxProgram(_bounds[type]);
    geometry->setIntersectionProgram(_intersects[type]);
    return geometry;
}

::optix::GeometryGroup OptiXContext::createGeometryGroup(const bool compact)
{
    auto group = _optixContext->createGeometryGroup();
    auto accel = _optixContext->createAcceleration(
        compact ? "Sbvh" : DEFAULT_ACCELERATION_STRUCTURE);
    accel->setProperty("vertex_buffer_name", "vertices_buffer");
    accel->setProperty("vertex_buffer_stride", "12");
    accel->setProperty("index_buffer_name", "indices_buffer");
    accel->setProperty("index_buffer_stride", "12");
    group->setAcceleration(accel);
    return group;
}

::optix::Group OptiXContext::createGroup()
{
    auto group = _optixContext->createGroup();
    group->setAcceleration(
        _optixContext->createAcceleration(DEFAULT_ACCELERATION_STRUCTURE));
    return group;
}
} // namespace brayns
