/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "PBRTModel.h"
#include "PBRTMaterial.h"
#include "util/PBRTSDFGeometryShape.h"
#include "util/Util.h"

#include <brayns/common/log.h>
#include <brayns/engine/Material.h>

#include <pbrtv2/core/bitarray.h>
#include <pbrtv2/core/paramset.h>
#include <pbrtv2/core/primitive.h>
#include <pbrtv2/core/transform.h>
#include <pbrtv2/core/volumeutil.h>

#include <pbrtv2/shapes/cone.h>
#include <pbrtv2/shapes/cylinder.h>
#include <pbrtv2/shapes/disk.h>
#include <pbrtv2/shapes/rectangle.h>
#include <pbrtv2/shapes/sphere.h>
#include <pbrtv2/shapes/trianglemesh.h>

#include <pbrtv2/volumes/fluorescent.h>
#include <pbrtv2/volumes/fluorescentannotatedgrid.h>
#include <pbrtv2/volumes/fluorescentbinarygrid.h>
#include <pbrtv2/volumes/fluorescentgrid.h>
#include <pbrtv2/volumes/fluorescentscattering.h>
#include <pbrtv2/volumes/fluorescentscatteringgrid.h>
#include <pbrtv2/volumes/grid.h>
#include <pbrtv2/volumes/heterogeneous.h>
#include <pbrtv2/volumes/homogeneous.h>
#include <pbrtv2/volumes/vsdgrid.h>

#include <cstring>

namespace brayns
{
pbrt::VolumeRegion* HomogeneusVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto g = static_cast<float>(meta.getProperty<double>("g", 0.0));
    const auto sigADArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {.0011f, .0024f, .014f});
    const auto sigSDArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {2.55f, 3.21f, 3.77f});
    const auto density = static_cast<float>(meta.getProperty<double>("density", 1.0));
    const auto LeDArr = meta.getProperty<std::array<double, 3>>("Le", {1., 1., 1.});
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});

    const float sigARGB[3] = {static_cast<float>(sigADArr[0]),
                              static_cast<float>(sigADArr[1]),
                              static_cast<float>(sigADArr[2])};
    const float sigSRGB[3] = {static_cast<float>(sigSDArr[0]),
                              static_cast<float>(sigSDArr[1]),
                              static_cast<float>(sigSDArr[2])};
    const float LeRGB[3] = {static_cast<float>(LeDArr[0]),
                            static_cast<float>(LeDArr[1]),
                            static_cast<float>(LeDArr[2])};

    const auto sigASpec = pbrt::Spectrum::FromRGB(sigARGB);
    const auto sigSSpec = pbrt::Spectrum::FromRGB(sigSRGB);
    const auto LeSpec = pbrt::Spectrum::FromRGB(LeRGB);
    pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                    static_cast<float>(p0Arr[1]),
                    static_cast<float>(p0Arr[2]));
    pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                    static_cast<float>(p1Arr[1]),
                    static_cast<float>(p1Arr[2]));

    return new pbrt::HomogeneousVolumeDensity(sigASpec, sigSSpec, density, g, LeSpec,
                                              pbrt::BBox(p0, p1), *otw);
}

pbrt::VolumeRegion* HeterogeneusVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto g = static_cast<float>(meta.getProperty<double>("g", 0.0));
    const auto sigADArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {.0011f, .0024f, .014f});
    const auto sigSDArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {2.55f, 3.21f, 3.77f});
    const auto minDensity = static_cast<float>(meta.getProperty<double>("min_density", 0.0));
    const auto maxDensity = static_cast<float>(meta.getProperty<double>("max_density", 1.0));
    const auto LeDArr = meta.getProperty<std::array<double, 3>>("Le", {1., 1., 1.});
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});

    const float sigARGB[3] = {static_cast<float>(sigADArr[0]),
                              static_cast<float>(sigADArr[1]),
                              static_cast<float>(sigADArr[2])};
    const float sigSRGB[3] = {static_cast<float>(sigSDArr[0]),
                              static_cast<float>(sigSDArr[1]),
                              static_cast<float>(sigSDArr[2])};
    const float LeRGB[3] = {static_cast<float>(LeDArr[0]),
                            static_cast<float>(LeDArr[1]),
                            static_cast<float>(LeDArr[2])};

    const auto sigASpec = pbrt::Spectrum::FromRGB(sigARGB);
    const auto sigSSpec = pbrt::Spectrum::FromRGB(sigSRGB);
    const auto LeSpec = pbrt::Spectrum::FromRGB(LeRGB);
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));

    return new pbrt::HeterogeneousVolumeDensity(sigASpec, sigSSpec, minDensity, maxDensity, g,
                                                LeSpec, pbrt::BBox(p0, p1), *otw);
}

pbrt::VolumeRegion* GridVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto g = static_cast<float>(meta.getProperty<double>("g", 0.0));
    const auto sigADArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {.0011f, .0024f, .014f});
    const auto sigSDArr = meta.getProperty<std::array<double, 3>>("sig_a",
                                                                  {2.55f, 3.21f, 3.77f});
    const auto density = meta.getProperty<std::vector<float>>("density", std::vector<float>());
    const auto nx = meta.getProperty<int>("nx", 0);
    const auto ny = meta.getProperty<int>("ny", 0);
    const auto nz = meta.getProperty<int>("nz", 0);
    const auto LeDArr = meta.getProperty<std::array<double, 3>>("Le", {1., 1., 1.});
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});

    const float sigARGB[3] = {static_cast<float>(sigADArr[0]),
                              static_cast<float>(sigADArr[1]),
                              static_cast<float>(sigADArr[2])};
    const float sigSRGB[3] = {static_cast<float>(sigSDArr[0]),
                              static_cast<float>(sigSDArr[1]),
                              static_cast<float>(sigSDArr[2])};
    const float LeRGB[3] = {static_cast<float>(LeDArr[0]),
                            static_cast<float>(LeDArr[1]),
                            static_cast<float>(LeDArr[2])};

    const auto sigASpec = pbrt::Spectrum::FromRGB(sigARGB);
    const auto sigSSpec = pbrt::Spectrum::FromRGB(sigSRGB);
    const auto LeSpec = pbrt::Spectrum::FromRGB(LeRGB);
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));

    const auto gridType = meta.getProperty<std::string>("grid_type", "grid");
    pbrt::VolumeRegion* result = nullptr;

    if(gridType == std::string("grid"))
        result = new pbrt::VolumeGrid(sigASpec, sigSSpec, g, LeSpec, pbrt::BBox(p0, p1), *otw,
                                      static_cast<uint64_t>(nx), static_cast<uint64_t>(ny),
                                      static_cast<uint64_t>(nz), density.data());
    else if(gridType == std::string("vsd"))
    {
        result = new pbrt::VSDVolumeGrid(sigASpec, sigSSpec, g, LeSpec, pbrt::BBox(p0, p1), *otw,
                                            static_cast<uint64_t>(nx), static_cast<uint64_t>(ny),
                                            static_cast<uint64_t>(nz), density.data());
    }

    return result;
}

pbrt::VolumeRegion* FluorescentVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));
    const auto fexArr = meta.getProperty<std::array<double, 3>>("fex");
    const float fexRGB[3] = {static_cast<float>(fexArr[0]),
                             static_cast<float>(fexArr[1]),
                             static_cast<float>(fexArr[2])};
    const auto fex = pbrt::Spectrum::FromRGB(fexRGB);
    const auto femArr = meta.getProperty<std::array<double, 3>>("fem");
    const float femRGB[3] = {static_cast<float>(femArr[0]),
                             static_cast<float>(femArr[1]),
                             static_cast<float>(femArr[2])};
    const auto fem = pbrt::Spectrum::FromRGB(femRGB);

    const auto epsilon = static_cast<float>(meta.getProperty<double>("epsilon"));
    const auto c = static_cast<float>(meta.getProperty<double>("c"));
    const auto yield = static_cast<float>(meta.getProperty<double>("yield"));
    const auto gf = static_cast<float>(meta.getProperty<double>("gf"));

    return new pbrt::FluorescentVolumeDensity(pbrt::BBox(p0, p1), *otw, fex, fem, epsilon, c,
                                              yield, gf);
}

pbrt::VolumeRegion*
FluorescentAnnotatedVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));
    const auto ntags = static_cast<uint64_t>(meta.getProperty<int>("ntags"));
    const auto& femV = meta.getPropertyRef<std::vector<float>>("fems");
    const auto& fexV = meta.getPropertyRef<std::vector<float>>("fexs");
    const auto& epsilonV = meta.getPropertyRef<std::vector<float>>("epsilons");
    const auto& cV = meta.getPropertyRef<std::vector<float>>("cs");
    const auto& yieldV = meta.getPropertyRef<std::vector<float>>("yields");
    const auto& gfV = meta.getPropertyRef<std::vector<float>>("gfs");

    pbrt::Spectrum* fem = new pbrt::Spectrum[femV.size()];
    pbrt::Spectrum* fex = new pbrt::Spectrum[fexV.size()];

    for(uint64_t i = 0; i < ntags; ++i)
    {
        const auto idx = i * 3;
        const float rgbFem[3] = {femV[idx], femV[idx+1], femV[idx+2]};
        fem[i] = pbrt::Spectrum::FromRGB(rgbFem);
        const float rgbFex[3] = {fexV[idx], fexV[idx+1], fexV[idx+2]};
        fex[i] = pbrt::Spectrum::FromRGB(rgbFex);
    }

    float* epsilon = new float[epsilonV.size()];
    float* c = new float[cV.size()];
    float* yield = new float[yieldV.size()];
    float* gf = new float[gfV.size()];

    std::memcpy(fem, femV.data(), sizeof(float)*femV.size());
    std::memcpy(fex, fexV.data(), sizeof(float)*fexV.size());
    std::memcpy(epsilon, epsilonV.data(), sizeof(float)*epsilonV.size());
    std::memcpy(c, cV.data(), sizeof(float)*cV.size());
    std::memcpy(yield, yieldV.data(), sizeof(float)*yieldV.size());
    std::memcpy(gf, gfV.data(), sizeof(float)*gfV.size());

    const auto& prefix = meta.getPropertyRef<std::string>("prefix");

    uint64_t nx, ny, nz;
    uint8_t* indices = pbrt::ReadIndices(prefix, nx, ny, nz);

    return new pbrt::FluorescentAnnotatedVolumeGrid(pbrt::BBox(p0, p1), *otw, fex, fem, epsilon,
                                                    c, yield, gf, nx, ny, nz, indices, ntags);
}

pbrt::VolumeRegion* FluorescentBinaryVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));
    const auto fexArr = meta.getProperty<std::array<double, 3>>("fex");
    const float fexRGB[3] = {static_cast<float>(fexArr[0]),
                             static_cast<float>(fexArr[1]),
                             static_cast<float>(fexArr[2])};
    const auto fex = pbrt::Spectrum::FromRGB(fexRGB);
    const auto femArr = meta.getProperty<std::array<double, 3>>("fem");
    const float femRGB[3] = {static_cast<float>(femArr[0]),
                             static_cast<float>(femArr[1]),
                             static_cast<float>(femArr[2])};
    const auto fem = pbrt::Spectrum::FromRGB(femRGB);

    const auto epsilon = static_cast<float>(meta.getProperty<double>("epsilon"));
    const auto c = static_cast<float>(meta.getProperty<double>("c"));
    const auto yield = static_cast<float>(meta.getProperty<double>("yield"));
    const auto gf = static_cast<float>(meta.getProperty<double>("gf"));

    const auto& prefix = meta.getPropertyRef<std::string>("prefix");
    uint64_t nx, ny, nz;
    pbrt::BitArray* density = pbrt::ReadBinaryVolume(prefix, nx, ny, nz);

    return new pbrt::FluorescentBinaryVolumeGrid(pbrt::BBox(p0, p1), *otw, nx, ny, nz, density,
                                                 fex, fem, epsilon, c, yield, gf);
}

pbrt::VolumeRegion* FluorescentGridVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));
    const auto fexArr = meta.getProperty<std::array<double, 3>>("fex");
    const float fexRGB[3] = {static_cast<float>(fexArr[0]),
                             static_cast<float>(fexArr[1]),
                             static_cast<float>(fexArr[2])};
    const auto fex = pbrt::Spectrum::FromRGB(fexRGB);
    const auto femArr = meta.getProperty<std::array<double, 3>>("fem");
    const float femRGB[3] = {static_cast<float>(femArr[0]),
                             static_cast<float>(femArr[1]),
                             static_cast<float>(femArr[2])};
    const auto fem = pbrt::Spectrum::FromRGB(femRGB);

    const auto epsilon = static_cast<float>(meta.getProperty<double>("epsilon"));
    const auto c = static_cast<float>(meta.getProperty<double>("c"));
    const auto yield = static_cast<float>(meta.getProperty<double>("yield"));
    const auto gf = static_cast<float>(meta.getProperty<double>("gf"));

    uint8_t* data;
    uint64_t nx, ny, nz;
    if(meta.hasProperty("prefix"))
    {
        const auto& prefix = meta.getPropertyRef<std::string>("prefix");
        data = pbrt::ReadIntVolume(prefix, nx, ny, nz);
    }
    else
    {
        nx = static_cast<uint64_t>(meta.getProperty<int>("nx", 0));
        ny = static_cast<uint64_t>(meta.getProperty<int>("ny", 0));
        nz = static_cast<uint64_t>(meta.getProperty<int>("nz", 0));
        const auto& fdata = meta.getProperty<std::vector<float>>("density");
        const auto size = nx*ny*nz;
        data = new uint8_t[size];
        for(uint64_t i = 0; i < size; ++i)
            data[i] = static_cast<uint8_t>(::Clamp(fdata[i], 0.f, 255.f));
    }

    return new pbrt::FluorescentGridDensity(pbrt::BBox(p0, p1), *otw, nx, ny, nz, data,
                                            fex, fem, epsilon, c, yield, gf);
}

pbrt::VolumeRegion*
FluorescentScatteringVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));
    const auto fexArr = meta.getProperty<std::array<double, 3>>("fex");
    const float fexRGB[3] = {static_cast<float>(fexArr[0]),
                             static_cast<float>(fexArr[1]),
                             static_cast<float>(fexArr[2])};
    const auto fex = pbrt::Spectrum::FromRGB(fexRGB);
    const auto femArr = meta.getProperty<std::array<double, 3>>("fem");
    const float femRGB[3] = {static_cast<float>(femArr[0]),
                             static_cast<float>(femArr[1]),
                             static_cast<float>(femArr[2])};
    const auto fem = pbrt::Spectrum::FromRGB(femRGB);
    const auto sigaArr = meta.getProperty<std::array<double, 3>>("a");
    const float sigaRGB[3] = {static_cast<float>(sigaArr[0]),
                              static_cast<float>(sigaArr[1]),
                              static_cast<float>(sigaArr[2])};
    const auto siga = pbrt::Spectrum::FromRGB(sigaRGB);
    const auto sigsArr = meta.getProperty<std::array<double, 3>>("s");
    const float sigsRGB[3] = {static_cast<float>(sigsArr[0]),
                              static_cast<float>(sigsArr[1]),
                              static_cast<float>(sigsArr[2])};
    const auto sigs = pbrt::Spectrum::FromRGB(sigsRGB);
    const auto LeArr = meta.getProperty<std::array<double, 3>>("Le");
    const float LeRGB[3] = {static_cast<float>(LeArr[0]),
                            static_cast<float>(LeArr[1]),
                            static_cast<float>(LeArr[2])};
    const auto Le = pbrt::Spectrum::FromRGB(LeRGB);

    const auto mweight = static_cast<float>(meta.getProperty<double>("mweight"));
    const auto epsilon = static_cast<float>(meta.getProperty<double>("epsilon"));
    const auto c = static_cast<float>(meta.getProperty<double>("c"));
    const auto yield = static_cast<float>(meta.getProperty<double>("yield"));
    const auto sscale = static_cast<float>(meta.getProperty<double>("sscale"));
    const auto fscale = static_cast<float>(meta.getProperty<double>("fscale"));
    const auto g = static_cast<float>(meta.getProperty<double>("g"));
    const auto gf = static_cast<float>(meta.getProperty<double>("gf"));
    const auto density = static_cast<float>(meta.getProperty<double>("density"));

    return new pbrt::FluorescentScatteringVolumeDensity(siga, sigs, g, Le, pbrt::BBox(p0, p1),
                                                        *otw, fex, fem, mweight, epsilon, c,
                                                        yield, gf, sscale, fscale, density);
}

pbrt::VolumeRegion*
FluorescentScatteringGridVolumeFactory(pbrt::Transform* otw, const PropertyMap& meta)
{
    const auto p0Arr = meta.getProperty<std::array<double, 3>>("p0", {0., 0., 0.});
    const auto p1Arr = meta.getProperty<std::array<double, 3>>("p1", {1., 1., 1.});
    const pbrt::Point p0 (static_cast<float>(p0Arr[0]),
                          static_cast<float>(p0Arr[1]),
                          static_cast<float>(p0Arr[2]));
    const pbrt::Point p1 (static_cast<float>(p1Arr[0]),
                          static_cast<float>(p1Arr[1]),
                          static_cast<float>(p1Arr[2]));
    const auto fexArr = meta.getProperty<std::array<double, 3>>("fex");
    const float fexRGB[3] = {static_cast<float>(fexArr[0]),
                             static_cast<float>(fexArr[1]),
                             static_cast<float>(fexArr[2])};
    const auto fex = pbrt::Spectrum::FromRGB(fexRGB);
    const auto femArr = meta.getProperty<std::array<double, 3>>("fem");
    const float femRGB[3] = {static_cast<float>(femArr[0]),
                             static_cast<float>(femArr[1]),
                             static_cast<float>(femArr[2])};
    const auto fem = pbrt::Spectrum::FromRGB(femRGB);
    const auto sigaArr = meta.getProperty<std::array<double, 3>>("a");
    const float sigaRGB[3] = {static_cast<float>(sigaArr[0]),
                              static_cast<float>(sigaArr[1]),
                              static_cast<float>(sigaArr[2])};
    const auto siga = pbrt::Spectrum::FromRGB(sigaRGB);
    const auto sigsArr = meta.getProperty<std::array<double, 3>>("s");
    const float sigsRGB[3] = {static_cast<float>(sigsArr[0]),
                              static_cast<float>(sigsArr[1]),
                              static_cast<float>(sigsArr[2])};
    const auto sigs = pbrt::Spectrum::FromRGB(sigsRGB);
    const auto LeArr = meta.getProperty<std::array<double, 3>>("Le");
    const float LeRGB[3] = {static_cast<float>(LeArr[0]),
                            static_cast<float>(LeArr[1]),
                            static_cast<float>(LeArr[2])};
    const auto Le = pbrt::Spectrum::FromRGB(LeRGB);

    const auto mweight = static_cast<float>(meta.getProperty<double>("mweight"));
    const auto epsilon = static_cast<float>(meta.getProperty<double>("epsilon"));
    const auto c = static_cast<float>(meta.getProperty<double>("c"));
    const auto yield = static_cast<float>(meta.getProperty<double>("yield"));
    const auto sscale = static_cast<float>(meta.getProperty<double>("sscale"));
    const auto fscale = static_cast<float>(meta.getProperty<double>("fscale"));
    const auto g = static_cast<float>(meta.getProperty<double>("g"));
    const auto gf = static_cast<float>(meta.getProperty<double>("gf"));

    uint64_t nx, ny, nz;
    float* data;
    if(meta.hasProperty("prefix"))
    {
        const auto& prefix = meta.getPropertyRef<std::string>("prefix");
        data = pbrt::ReadFloatVolume(prefix, nx, ny, nz);
    }
    else
    {
        nx = static_cast<uint64_t>(meta.getProperty<int>("nx"));
        ny = static_cast<uint64_t>(meta.getProperty<int>("ny"));
        nz = static_cast<uint64_t>(meta.getProperty<int>("nz"));
        data = new float[nx*ny*nz];
        const auto& dataV = meta.getPropertyRef<std::vector<float>>("density");
        std::memcpy(data, dataV.data(), dataV.size() * sizeof(float));
    }

    return new pbrt::FluorescentScatteringGridDensity(siga, sigs, g, Le, pbrt::BBox(p0, p1), *otw,
                                                      nx, ny, nz, data, fex, fem, epsilon, c,
                                                      yield, gf);

}

std::unordered_map<std::string, PBRTModel::VolumeFactory> PBRTModel::_volumeFactories = {
    {"homogeneus", HomogeneusVolumeFactory},
    {"heterogeneus", HeterogeneusVolumeFactory},
    {"grid", GridVolumeFactory},
    {"fluorescence", FluorescentVolumeFactory},
    {"fluorescence_annotated", FluorescentAnnotatedVolumeFactory},
    {"fluorescence_binary", FluorescentBinaryVolumeFactory},
    {"fluorescence_grid", FluorescentGridVolumeFactory},
    {"fluorescence_scattering", FluorescentScatteringVolumeFactory},
    {"fluorescence_scattering_grid", FluorescentScatteringGridVolumeFactory}
};


PBRTModel::PBRTModel(AnimationParameters& animationParameters,
                     VolumeParameters& volumeParameters)
    : Model(animationParameters, volumeParameters)
{
}

PBRTModel::~PBRTModel()
{
}

void PBRTModel::commitGeometry()
{
    // DO NOTHING, WE NEED THE TRANSFORMATION AT THE
    // CREATION TIME OF THE SHAPES.
}

std::vector<pbrt::Reference<pbrt::Primitive>>
PBRTModel::commitToPBRT(const Transformation &transform, std::vector<pbrt::Sensor*>& sensorBuffer,
                        std::vector<pbrt::VolumeRegion*>& volumeBuffer)
{
    _commitMaterials();

    _objectToWorld = pbrtTransform(transform);
    _worldToObject = pbrt::Transform (_objectToWorld.GetInverseMatrix(),
                                      _objectToWorld.GetMatrix());

    auto spheres = _createSpheres(&_objectToWorld, &_worldToObject);
    auto cylinders = _createCylinders(&_objectToWorld, &_worldToObject);
    auto cones = _createCones(&_objectToWorld, &_worldToObject);
    auto meshes = _createMeshes(&_objectToWorld, &_worldToObject);
    auto sdfGeoms = _createSDFGeometries(&_objectToWorld, &_worldToObject);

    Primitives result;
    result.insert(result.end(), spheres.begin(), spheres.end());
    spheres.clear();
    result.insert(result.end(), cylinders.begin(), cylinders.end());
    cylinders.clear();
    result.insert(result.end(), cones.begin(), cones.end());
    cones.clear();
    result.insert(result.end(), meshes.begin(), meshes.end());
    meshes.clear();
    result.insert(result.end(), sdfGeoms.begin(), sdfGeoms.end());
    sdfGeoms.clear();

    auto sensorList = _createSensors(&_objectToWorld, &_worldToObject);
    sensorBuffer.insert(sensorBuffer.end(), sensorList.begin(), sensorList.end());

    auto volumeList = _createVolumes(&_objectToWorld);
    volumeBuffer.insert(volumeBuffer.end(), volumeList.begin(), volumeList.end());

    updateBounds();
    _markGeometriesClean();
    _instancesDirty = false;

    return result;
}

void PBRTModel::buildBoundingBox()
{
    auto material = createMaterial(BOUNDINGBOX_MATERIAL_ID, "bounding_box");
    material->setDiffuseColor({1, 1, 1});
    material->setEmission(1.);
    material->commit();
}

SharedDataVolumePtr PBRTModel::createSharedDataVolume(const Vector3ui &dimensions,
                                                      const Vector3f &spacing,
                                                      const DataType type) const
{
    (void) dimensions;
    (void) spacing;
    (void) type;
    return SharedDataVolumePtr{nullptr};
}

MaterialPtr PBRTModel::createMaterialImpl(const PropertyMap& properties)
{
    return std::shared_ptr<Material>(new PBRTMaterial(properties));
}

bool PBRTModel::materialsDirty() const
{
    for(const auto& m : getMaterials())
        if(m.second->isModified())
            return true;

    return false;
}

void PBRTModel::_commitMaterials()
{
    for(auto m : _materials)
    {
        PBRTMaterial* pbrtM = static_cast<PBRTMaterial*>(m.second.get());
        if(!pbrtM)
            continue;

        pbrtM->commit();
    }
}

std::vector<pbrt::VolumeRegion*> PBRTModel::_createVolumes(pbrt::Transform* otw)
{
    std::vector<pbrt::VolumeRegion*> result;

    for(const auto& metaObjectList : getMetaObjects())
    {
        for(const auto& metaObject : metaObjectList.second)
        {
            // Only interested in volume metaobjects
            if(!metaObject.hasProperty("volume_type"))
                continue;

            // Attempt to create a volume if the type is correct and all the
            // parameters fulfill the specifications
            const auto volType = metaObject.getProperty<std::string>("volume_type");
            auto factorIt = _volumeFactories.find(volType);
            if(factorIt == _volumeFactories.end())
                continue;

            pbrt::VolumeRegion* volumePtr = factorIt->second(otw, metaObject);
            if(volumePtr != nullptr)
                result.push_back(volumePtr);
        }
    }

    return result;
}

std::vector<pbrt::Reference<pbrt::Primitive>>
PBRTModel::_createSpheres(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    pbrt::AreaLight* dummyAL = nullptr;

    for(const auto& sphereList : getSpheres())
    {
        const auto& mat = getMaterial(sphereList.first);
        PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());

        for(const auto& sphere : sphereList.second)
        {
            pbrt::ParamSet params;

            const auto otwS = pbrt::Translate(pbrt::Vector(sphere.center.x,
                                                           sphere.center.y,
                                                           sphere.center.z));

            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              otwS.GetMatrix())));
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              otwS.GetInverseMatrix())));

            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

           params.AddFloat("radius", &sphere.radius, 1);

            auto sphereShape = pbrt::CreateSphereShape(otwFinalPtr, wtoFinalPtr, false, params);

            result.push_back(pbrt::Reference<pbrt::Primitive>(
                                 new pbrt::GeometricPrimitive(sphereShape,
                                                              impl.getPBRTMaterial(),
                                                              dummyAL)));
        }
    }

    return result;
}

std::vector<pbrt::Reference<pbrt::Primitive>>
PBRTModel::_createCylinders(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    pbrt::AreaLight* dummyAL = nullptr;

    for(const auto& cylinderList : getCylinders())
    {
        const auto& mat = getMaterial(cylinderList.first);
        PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());

        for(const auto& cylinder : cylinderList.second)
        {
            // Compute PBRT Transform from Brayns Transform
            const auto direction = glm::normalize(cylinder.up - cylinder.center);
            glm::vec3 theoreticalUp (0.f, 1.f, 0.f);
            if (direction == theoreticalUp)
                theoreticalUp = glm::vec3(1.f, 0.f, 0.f);
            auto rUp = glm::normalize(glm::cross(direction, theoreticalUp));
            auto pbrtMat = pbrt::LookAt(pbrt::Point(cylinder.center.x,
                                                    cylinder.center.y,
                                                    cylinder.center.z),
                                        pbrt::Point(cylinder.up.x,
                                                    cylinder.up.y,
                                                    cylinder.up.z),
                                        pbrt::Vector(rUp.x, rUp.y, rUp.z));

            const pbrt::Transform cylTrans(pbrtMat);

            // Multiply by model transformation
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              cylTrans.GetMatrix())));
            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              cylTrans.GetInverseMatrix())));

            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

            // Build cylinder paramset
            pbrt::ParamSet params;

            params.AddFloat("radius", &cylinder.radius, 1);

            auto zmin = 0.f;
            params.AddFloat("zmin", &zmin, 1);

            auto zmax = glm::length(cylinder.up - cylinder.center);
            params.AddFloat("zmax", &zmax, 1);

            auto cylinderShape = pbrt::CreateCylinderShape(otwFinalPtr, wtoFinalPtr,
                                                           false, params);

            result.push_back(pbrt::Reference<pbrt::Primitive>(
                                 new pbrt::GeometricPrimitive(cylinderShape,
                                                              impl.getPBRTMaterial(),
                                                              dummyAL)));
        }
    }

    return result;
}

std::vector<pbrt::Reference<pbrt::Primitive>>
PBRTModel::_createCones(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    pbrt::AreaLight* dummyAL = nullptr;

    for(const auto& coneList : getCones())
    {
        const auto& mat = getMaterial(coneList.first);
        PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());

        for(const auto& cone : coneList.second)
        {
            // Compute PBRT Transform from Brayns Transform
            const auto direction = glm::normalize(cone.up - cone.center);
            glm::vec3 theoreticalUp (0.f, 1.f, 0.f);
            if (direction == theoreticalUp)
                theoreticalUp = glm::vec3(1.f, 0.f, 0.f);
            auto rUp = glm::normalize(glm::cross(direction, theoreticalUp));
            auto pbrtMat = pbrt::LookAt(pbrt::Point(cone.center.x, cone.center.y, cone.center.z),
                                        pbrt::Point(cone.up.x, cone.up.y, cone.up.z),
                                        pbrt::Vector(rUp.x, rUp.y, rUp.z));

            const pbrt::Transform conTrans(pbrtMat);

            // Multiply by model transformation
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              conTrans.GetMatrix())));
            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              conTrans.GetInverseMatrix())));

            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

            // Build cone paramset
            pbrt::ParamSet params;

            params.AddFloat("radius", &cone.centerRadius, 1);

            auto height = glm::length(cone.up - cone.center);
            params.AddFloat("height", &height, 1);

            auto coneShape = pbrt::CreateConeShape(otwFinalPtr, wtoFinalPtr, false, params);

            result.push_back(pbrt::Reference<pbrt::Primitive>(
                                 new pbrt::GeometricPrimitive(coneShape,
                                                              impl.getPBRTMaterial(),
                                                              dummyAL)));
        }
    }

    return result;
}

std::vector<pbrt::Reference<pbrt::Primitive>>
PBRTModel::_createMeshes(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    pbrt::AreaLight* dummyAL = nullptr;

    for(const auto& meshList : getTriangleMeshes())
    {
        const auto& mat = getMaterial(meshList.first);
        PBRTMaterial* impl = static_cast<PBRTMaterial*>(mat.get());

        const auto& mesh = meshList.second;

        // Build mesh paramset
        pbrt::ParamSet params;

        // Triangles
        std::vector<int> indices (mesh.indices.size() * 3, 0);
        for(size_t i = 0; i < mesh.indices.size(); ++i)
        {
            const size_t idx = i * 3;
            indices[idx] = static_cast<int>(mesh.indices[i].x);
            indices[idx+1] = static_cast<int>(mesh.indices[i].y);
            indices[idx+2] = static_cast<int>(mesh.indices[i].z);
        }
        params.AddInt("indices", indices.data(), static_cast<int>(indices.size()));

        // Verties
        std::vector<pbrt::Point> vertices (mesh.vertices.size());
        for(size_t i = 0; i < mesh.vertices.size(); ++i)
            vertices[i] = pbrt::Point(mesh.vertices[i].x,
                                      mesh.vertices[i].y,
                                      mesh.vertices[i].z);
        params.AddPoint("P", vertices.data(), static_cast<int>(mesh.vertices.size()));

        // UVs
        if(!mesh.textureCoordinates.empty() &&
           mesh.textureCoordinates.size() == mesh.vertices.size())
        {
            std::vector<float> uvs (mesh.textureCoordinates.size() * 2);
            for(size_t i = 0; i < mesh.textureCoordinates.size(); ++i)
            {
                auto idx = i * 2;
                uvs[idx] = mesh.textureCoordinates[i].x;
                uvs[idx + 1] = mesh.textureCoordinates[i].y;
            }
            params.AddFloat("uv", uvs.data(), static_cast<int>(mesh.textureCoordinates.size()) * 2);
        }

        if(!mesh.normals.empty() && mesh.normals.size() == mesh.vertices.size())
        {
            std::vector<pbrt::Normal> normals (mesh.normals.size());
            for(size_t i = 0; i < mesh.normals.size(); ++i)
                normals[i] = pbrt::Normal(mesh.normals[i].x,
                                          mesh.normals[i].y,
                                          mesh.normals[i].z);
            params.AddNormal("N", normals.data(), static_cast<int>(mesh.normals.size()));
        }

        pbrt::Reference<pbrt::Shape> refShape (pbrt::CreateTriangleMeshShape(otw, wto, false, params));

        result.push_back(pbrt::Reference<pbrt::Primitive>(
                             new pbrt::GeometricPrimitive(refShape,
                                                          impl->getPBRTMaterial(),
                                                          dummyAL)));
    }

    return result;
}

std::vector<pbrt::Reference<pbrt::Primitive>>
PBRTModel::_createSDFGeometries(pbrt::Transform* otw, pbrt::Transform* wto)
{
    Primitives result;

    pbrt::AreaLight* dummyAL = nullptr;

    for(const auto& geometryList : getSDFGeometryData().geometryIndices)
    {
        const auto& mat = getMaterial(geometryList.first);
        PBRTMaterial& impl = static_cast<PBRTMaterial&>(*mat.get());

        for(const auto gemetryIndex : geometryList.second)
        {
            const SDFGeometry& geom = getSDFGeometryData().geometries[gemetryIndex];

            pbrt::Reference<pbrt::Shape> shape (nullptr);
            switch(geom.type)
            {
                case SDFType::Sphere:
                    shape = pbrt::Reference<pbrt::Shape>(new PBRTSDFGeometryShape<SDFType::Sphere>
                                                         (otw, wto, false, &geom, &getSDFGeometryData()));
                    break;
                case SDFType::Pill:
                    shape = pbrt::Reference<pbrt::Shape>(new PBRTSDFGeometryShape<SDFType::Pill>
                                                         (otw, wto, false, &geom, &getSDFGeometryData()));
                    break;
                case SDFType::ConePill:
                    shape = pbrt::Reference<pbrt::Shape>(new PBRTSDFGeometryShape<SDFType::ConePill>
                                                         (otw, wto, false, &geom, &getSDFGeometryData()));
                    break;
                case SDFType::ConePillSigmoid:
                    shape = pbrt::Reference<pbrt::Shape>(new PBRTSDFGeometryShape<SDFType::ConePillSigmoid>
                                                         (otw, wto, false, &geom, &getSDFGeometryData()));
                    break;

            }            

            result.push_back(pbrt::Reference<pbrt::Primitive>(
                                 new pbrt::GeometricPrimitive(shape,
                                                              impl.getPBRTMaterial(),
                                                              dummyAL)));
        }
    }

    return result;
}

std::vector<pbrt::Sensor*>
PBRTModel::_createSensors(pbrt::Transform* otw, pbrt::Transform* wto)
{
    std::vector<pbrt::Sensor*> result;

    for(const auto& metaObjList : _geometries->_metaObjects)
    {
        for(const auto& sensorProps : metaObjList.second)
        {
            if(!sensorProps.hasProperty("sensor_shape_type"))
                continue;

            // CHECK FOR VALID SENSOR SHAPE
            auto shapeStr = sensorProps.getProperty<std::string>("sensor_shape_type");
            if(shapeStr != std::string("rectangle") && shapeStr != std::string("disk"))
            {
                BRAYNS_WARN << "PBRTModel: Unknown sensor shape: " << shapeStr
                            << "Valid values: \"rectangle\"|\"disk\". Skipping"
                            << std::endl;
                continue;
            }

            // PREPARE TRANSFORM
            const auto sensorRotation =
                    sensorProps.getProperty<std::array<double, 4>>("sensor_rotation",
                                                                   {0.0,1.0,0.0,0.0});
            const auto angle = static_cast<float>(sensorRotation[0]);
            const pbrt::Vector axis (static_cast<float>(sensorRotation[1]),
                                     static_cast<float>(sensorRotation[2]),
                                     static_cast<float>(sensorRotation[3]));

            const auto sensorPos =
                   sensorProps.getProperty<std::array<double, 3>>("sensor_translation",
                                                                  {0.0,0.0,0.0});
            const pbrt::Vector trans (static_cast<float>(sensorPos[0]),
                                      static_cast<float>(sensorPos[1]),
                                      static_cast<float>(sensorPos[2]));
            const auto sensorScale =
                    sensorProps.getProperty<std::array<double, 3>>("sensor_scale",
                                                                   {1.0,1.0,1.0});
            const pbrt::Vector scale (static_cast<float>(sensorScale[0]),
                                      static_cast<float>(sensorScale[1]),
                                      static_cast<float>(sensorScale[2]));

            const auto localRot = pbrt::Rotate(angle, axis);
            const auto localTrans = pbrt::Translate(trans);
            const auto localScale = pbrt::Scale(scale.x, scale.y, scale.z);
            const auto localTransMatrix = pbrt::Matrix4x4::Mul(
                                            pbrt::Matrix4x4::Mul(
                                                            localRot.GetMatrix(),
                                                            localTrans.GetMatrix()),
                                                            localScale.GetMatrix());
            const pbrt::Transform localTransform (localTransMatrix);

            // Final transforms
            std::unique_ptr<pbrt::Transform> otwFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(otw->GetMatrix(),
                                                              localTransform.GetMatrix())));
            std::unique_ptr<pbrt::Transform> wtoFinal
                    (new pbrt::Transform(pbrt::Matrix4x4::Mul(wto->GetMatrix(),
                                                              localTransform.GetInverseMatrix())));

            // Final transforms points
            auto otwFinalPtr = otwFinal.get();
            auto wtoFinalPtr = wtoFinal.get();

            // Store final transform into pool
            _transformPool.push_back(std::move(otwFinal));
            _transformPool.push_back(std::move(wtoFinal));

            // CREATE SENSOR
            pbrt::Sensor* sensor = nullptr;

            const auto reference = sensorProps.getProperty<std::string>("reference", "sensor");
            const auto xPixels = sensorProps.getProperty<int>("xpixels", 0);
            const auto yPixels = sensorProps.getProperty<int>("ypixels", 0);
            const auto fov = static_cast<float>(sensorProps.getProperty<double>("fov", 45.));

            if(shapeStr == std::string("rectangle"))
            {
                const auto height = static_cast<float>(sensorProps.getProperty<double>(
                                                           "sensor_shape_height", 0.0));
                const auto x = static_cast<float>(sensorProps.getProperty<double>(
                                                      "sensor_shape_x", 1.0));
                const auto y = static_cast<float>(sensorProps.getProperty<double>(
                                                      "sensor_shape_y", 1.0));
                pbrt::Shape* shape = new pbrt::Rectangle(otwFinalPtr, wtoFinalPtr,
                                                         false, height, x, y);
                sensor = new pbrt::Sensor(shapeStr, reference, shape,
                                          xPixels, yPixels, x, y, fov);
            }
            else // disk otherwise
            {
                const auto height = static_cast<float>(sensorProps.getProperty<double>(
                                                           "sensor_shape_height", 0.0));
                const auto radius = static_cast<float>(sensorProps.getProperty<double>(
                                                      "sensor_shape_radius", 1.0));
                const auto innerrad = static_cast<float>(sensorProps.getProperty<double>(
                                                      "sensor_shape_innerradius", 0.0));
                const auto phi = static_cast<float>(sensorProps.getProperty<double>(
                                                      "sensor_shape_phimax", 360.0));

                pbrt::Shape* shape = new pbrt::Disk(otwFinalPtr, wtoFinalPtr,
                                                    false, height, radius, innerrad, phi);

                sensor = new pbrt::Sensor(shapeStr, reference, shape,
                                          xPixels, yPixels, 2 * radius, 2 * radius, fov);
            }

            result.push_back(sensor);
        }
    }

    return result;
}

}
