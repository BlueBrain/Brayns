/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "iblUtils.h"

#include "imageUtils.h"
#include <boost/progress.hpp>
#include <brayns/common/Timer.h>
#include <brayns/common/log.h>
#include <brayns/common/material/Texture2D.h>

namespace brayns
{
namespace
{
Vector3f polarToCartesian(const Vector2f& uv)
{
    const float theta = (uv.x - 0.5f) * (M_PI * 2.f);
    const float phi = uv.y * M_PI;
    const float sinPhi = std::sin(phi);
    const Vector3f n(sinPhi * std::cos(theta), -std::cos(phi),
                     sinPhi * std::sin(theta));
    return glm::normalize(n);
}

const float RECIPROCAL_PI2 = 0.15915494f;

Vector2f cartesianToPolar(const Vector3f& n)
{
    Vector2f uv;
    uv.x = std::atan2(n.z, n.x) * RECIPROCAL_PI2 + 0.5f;
    uv.y = std::asin(n.y) * M_1_PI + 0.5f;
    return uv;
}

inline Vector3f bilerp(const Vector2f& frac, const Vector3f& c00,
                       const Vector3f& c01, const Vector3f& c10,
                       const Vector3f& c11)
{
    return glm::lerp(glm::lerp(c00, c01, frac.x), glm::lerp(c10, c11, frac.x),
                     frac.y);
}

struct BilinCoords
{
    Vector2i st0;
    Vector2i st1;
    Vector2f frac;
};

inline Vector2f frac(const Vector2f& x)
{
    return {x.x - std::floor(x.x), x.y - std::floor(x.y)};
}

inline BilinCoords bilinear_coords(const Vector2i& size, const Vector2f& p)
{
    BilinCoords coords;

    // repeat: get remainder within [0..1] parameter space
    // lower sample shifted by half a texel
    const Vector2f halfTexel(0.5f / size.x, 0.5f / size.y);
    Vector2f tc = frac(p - halfTexel);
    tc = max(tc, Vector2f(0.0f)); // filter out inf/NaN

    // scale by texture size
    const Vector2f sizef(nextafter((float)size.x, -1.0f),
                         nextafter((float)size.y, -1.0f));
    tc = tc * sizef;
    coords.frac = frac(tc);

    coords.st0 = Vector2f(tc);
    coords.st1 = coords.st0 + 1;
    // handle border cases
    if (coords.st1.x >= size.x)
        coords.st1.x = 0;
    if (coords.st1.y >= size.y)
        coords.st1.y = 0;

    return coords;
}

Vector3f getTexel(const Texture2D& tex, const Vector2i& uv)
{
    const auto index = (uv.x + uv.y * tex.getWidth()) * tex.getNbChannels();
    switch (tex.getDepth())
    {
    case 1:
    {
        const auto ptr = &tex.getRawData()[index];
        return {*ptr / 255.f, *(ptr + 1) / 255.f, *(ptr + 2) / 255.f};
    }
    case 4:
    {
        const auto ptr = &tex.getRawData<float>()[index];
        return {*ptr, *(ptr + 1), *(ptr + 2)};
    }
    default:
        throw std::runtime_error(
            "Depths other than 1 and 4 are not supported for IBL");
    }
}

Vector3f tex2D(const Texture2D& tex, const Vector2f& uv)
{
    BilinCoords cs = bilinear_coords({tex.getWidth(), tex.getHeight()}, uv);

    const Vector3f c00 = getTexel(tex, cs.st0);
    const Vector3f c01 = getTexel(tex, {cs.st1.x, cs.st0.y});
    const Vector3f c10 = getTexel(tex, {cs.st0.x, cs.st1.y});
    const Vector3f c11 = getTexel(tex, cs.st1);

    return bilerp(cs.frac, c00, c01, c10, c11);
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

Vector2f Hammersley(const uint i, const uint N)
{
    return {float(i) / float(N), RadicalInverse_VdC(i)};
}

Vector3f importanceSampleGGX(const Vector2f& uv, const Vector3f& N,
                             const float roughness)
{
    const float a = roughness * roughness;
    const float phi = 2.0f * M_PI * uv.x;
    const float cosTheta =
        std::sqrt((1.0f - uv.y) / (1.0f + (a * a - 1.0f) * uv.y));
    const float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

    const Vector3f H(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta,
                     cosTheta);

    // from tangent-space vector to world-space sample vector
    const Vector3f up = std::abs(N.z) < 0.999f ? Vector3f(0.0f, 0.0f, 1.0f)
                                               : Vector3f(1.0f, 0.0f, 0.0f);
    const Vector3f tangent = normalize(cross(up, N));
    const Vector3f bitangent = cross(N, tangent);

    const Vector3f sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return glm::normalize(sampleVec);
}

float GeometrySchlickGGX(const float NdotV, const float roughness)
{
    // note that we use a different k for IBL
    const float a = roughness;
    const float k = (a * a) / 2.0f;

    const float nom = NdotV;
    const float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(const Vector3f& N, const Vector3f& V, const Vector3f& L,
                    const float roughness)
{
    const float NdotV = std::max(dot(N, V), 0.0f);
    const float NdotL = std::max(dot(N, L), 0.0f);
    const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    const float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

#ifdef HAVE_TEX2LOD
float DistributionGGX(const Vector3f& N, const Vector3f& H,
                      const float roughness)
{
    const float a = roughness * roughness;
    const float a2 = a * a;
    const float NdotH = std::max(glm::dot(N, H), 0.0f);
    const float NdotH2 = NdotH * NdotH;

    const float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = M_PI * denom * denom;

    return nom / denom;
}
#endif

void saveToFile(const std::vector<Vector3f>& texture, const size_t width,
                const size_t height, const std::string& outfile)
{
    auto img =
        FreeImage_AllocateT(FIT_RGBF, width, height, 8 * 3 * sizeof(float));
    const Vector3f* src = texture.data();
    for (unsigned y = 0; y < height; y++)
    {
        float* dst_bits = (float*)FreeImage_GetScanLine(img, y);
        memcpy(dst_bits, src, width * 3 * sizeof(float));
        src += width;
    }
    FreeImage_Save(FIF_HDR, img, (outfile + ".hdr").c_str());
    FreeImage_Unload(img);
}

void saveToFile(const Texture2D tex, const size_t width, const size_t height,
                const std::string& outfile)
{
    std::vector<Vector3f> buffer(width * height);
    memcpy(buffer.data(), tex.getRawData(), width * height * 3 * sizeof(float));
    saveToFile(buffer, width, height, outfile);
}

} // anonymous namespace

namespace iblUtils
{
void computeIrradianceMap(const Texture2D& tex, const std::string& filename)
{
    const size_t width = 512;
    const size_t height = 256;
    boost::progress_display progress(height);
    std::vector<Vector3f> outtexture(width * height);
    Timer timer;
    timer.start();
    for (size_t y = 0; y < height; ++y)
    {
#pragma omp parallel for
        for (size_t x = 0; x < width; ++x)
        {
            const Vector2f uv(float(x) / width, float(y) / height);
            const Vector3f N = polarToCartesian(uv);

            Vector3f irradiance;
            Vector3f up(0.f, 1.f, 0.f);
            if (N.y == -1.0f)
            {
                up = Vector3f(0.0f, 0.0f, -1.0f);
            }

            const Vector3f right = glm::cross(up, N);
            up = glm::cross(N, right);
            const float sampleDelta = 0.01f;
            size_t samples = 0;
            for (float phi = 0.0f; phi < M_PI * 2.f; phi += sampleDelta)
            {
                const float sinPhi = std::sin(phi);
                const float cosPhi = std::cos(phi);
                for (float theta = 0.0f; theta < M_PI_2; theta += sampleDelta)
                {
                    const float sinTheta = std::sin(theta);
                    const float cosTheta = std::cos(theta);
                    const Vector3f tangentSample(sinTheta * cosPhi,
                                                 sinTheta * sinPhi, cosTheta);
                    const Vector3f dir = glm::normalize(
                        tangentSample.x * right + tangentSample.y * up +
                        tangentSample.z * N);

                    irradiance +=
                        tex2D(tex, cartesianToPolar(dir)) * cosTheta * sinTheta;
                    samples++;
                }
            }
            irradiance = M_PI * irradiance / (float)samples;
            outtexture[y * width + x] = irradiance;
        }
        ++progress;
    }
    BRAYNS_INFO << "Irradiance map computed in " << timer.elapsed()
                << " seconds" << std::endl;
    saveToFile(outtexture, width, height, filename);
}

void computeRadianceMap(const Texture2D& tex, const size_t mip,
                        const float roughness, const std::string& filename)
{
    const size_t width = tex.getWidth() * std::pow(0.5, mip);
    const size_t height = tex.getHeight() * std::pow(0.5, mip);
    boost::progress_display progress(height);
    std::vector<Vector3f> outtexture(width * height);
    const size_t SAMPLE_COUNT = 16384;
    Timer timer;
    timer.start();
    for (size_t y = 0; y < height; ++y)
    {
#pragma omp parallel for
        for (size_t x = 0; x < width; ++x)
        {
            const Vector2f uv(float(x) / width, float(y) / height);
            const Vector3f N = polarToCartesian(uv);

            Vector3f prefilteredColor;
            float totalWeight = 0.0f;

            for (size_t i = 0; i < SAMPLE_COUNT; ++i)
            {
                const Vector2f Xi = Hammersley(i, SAMPLE_COUNT);
                const Vector3f H = importanceSampleGGX(Xi, N, roughness);

                const Vector3f L =
                    glm::normalize(2.0f * glm::dot(N, H) * H - N);
                const float NdotL = std::max(glm::dot(N, L), 0.0f);
                if (NdotL > 0.0f)
                {
#ifdef HAVE_TEX2LOD
                    // sample from the environment's mip level based on
                    // roughness/pdf
                    const float D = DistributionGGX(N, H, roughness);
                    const float NdotH = std::max(glm::dot(N, H), 0.0f);
                    const float HdotV = std::max(glm::dot(H, N), 0.0f);
                    const float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;

                    const float saTexel =
                        4.0f * M_PI / (tex.getWidth() * tex.getHeight());
                    const float saSample =
                        1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

                    const float mipLevel =
                        roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
                    prefilteredColor +=
                        tex2DLod(tex, cartesianToPolar(L), mipLevel) * NdotL;
#else
                    prefilteredColor += tex2D(tex, cartesianToPolar(L)) * NdotL;
#endif
                    totalWeight += NdotL;
                }
            }
            outtexture[y * width + x] = prefilteredColor / totalWeight;
        }
        ++progress;
    }
    BRAYNS_INFO << "Radiance map " << mip << " computed in " << timer.elapsed()
                << " seconds" << std::endl;
    saveToFile(outtexture, width, height, filename + std::to_string(mip));
}

void computeRadianceMap(const Texture2D& tex, const std::string& filename)
{
    saveToFile(tex, tex.getWidth(), tex.getHeight(), filename);
    const size_t maxMipLevels = 5;
    for (size_t mip = 1; mip < maxMipLevels; ++mip)
    {
        const float roughness = (float)mip / (float)(maxMipLevels - 1);
        computeRadianceMap(tex, mip, roughness, filename);
    }
}

void computeBRDF(const std::string& filename)
{
    const size_t width = 512;
    const size_t height = 512;
    const uint SAMPLE_COUNT = 1024u;
    boost::progress_display progress(height);
    std::vector<Vector3f> outtexture(width * height);
    Timer timer;
    timer.start();
    for (size_t y = 0; y < height; ++y)
    {
#pragma omp parallel for
        for (size_t x = 0; x < width; ++x)
        {
            const float NdotV = std::max(0.001f, float(x) / width);
            const float roughness = float(y) / height;
            Vector3f V(std::sqrt(1.0f - NdotV * NdotV), 0.f, NdotV);

            float A = 0.0f;
            float B = 0.0f;

            Vector3f N(0.0f, 0.0f, 1.0f);
            for (uint i = 0u; i < SAMPLE_COUNT; ++i)
            {
                // generates a sample vector that's biased towards the
                // preferred alignment direction (importance sampling).
                const Vector2f Xi = Hammersley(i, SAMPLE_COUNT);
                const Vector3f H = importanceSampleGGX(Xi, N, roughness);
                const Vector3f L =
                    glm::normalize(2.0f * glm::dot(V, H) * H - V);

                const float NdotL = std::max(L.z, 0.0f);

                if (NdotL > 0.0f)
                {
                    const float NdotH = std::max(H.z, 0.0f);
                    const float VdotH = std::max(dot(V, H), 0.0f);
                    const float G = GeometrySmith(N, V, L, roughness);
                    const float G_Vis = (G * VdotH) / (NdotH * NdotV);
                    const float Fc = std::pow(1.0f - VdotH, 5.0f);

                    A += (1.0f - Fc) * G_Vis;
                    B += Fc * G_Vis;
                }
            }
            A /= float(SAMPLE_COUNT);
            B /= float(SAMPLE_COUNT);
            outtexture[y * width + x] = Vector3f(A, B, 0.f);
        }
        ++progress;
    }
    BRAYNS_INFO << "BRDF computed in " << timer.elapsed() << " seconds"
                << std::endl;
    saveToFile(outtexture, width, height, filename);
}
}
} // namespace brayns
