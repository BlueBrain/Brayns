/* Copyright (c) 2011-2018, EPFL/Blue Brain Project
 *                          Stanislaw Adaszewski <stanislaw.adaszewski@epfl.ch>
 *                          Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *                          Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *                          Raphael Dumusc <raphael.dumusc@epfl.ch>
 *                          Daniel Nachbaur<daniel.nachbaur@epfl.ch>
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

#include "BBICFile.h"

#include "lzfFilter/lzf_filter.h"

namespace bbic
{
constexpr char BBIC_DEFAULT_GROUP_NAME[] = "bbic/volumes/0";
constexpr char BBIC_ATTRIBUTE_WIDTH[] = "width";
constexpr char BBIC_ATTRIBUTE_HEIGHT[] = "height";
constexpr char BBIC_ATTRIBUTE_NUM_SLICES[] = "num_slices";
constexpr char BBIC_ATTRIBUTE_TILE_SIZE[] = "tile_size";
constexpr char BBIC_GROUP_LEVELS[] = "levels";

File::File(const std::string& file)
    : _file(std::make_unique<HighFive::File>(file))
    , _volGroup(_file->getGroup(BBIC_DEFAULT_GROUP_NAME))
{
    HighFive::SilenceHDF5 silence;
    _volGroup.getAttribute(BBIC_ATTRIBUTE_WIDTH).read(width_);
    _volGroup.getAttribute(BBIC_ATTRIBUTE_HEIGHT).read(height_);
    _volGroup.getAttribute(BBIC_ATTRIBUTE_NUM_SLICES).read(depth_);
    _volGroup.getAttribute(BBIC_ATTRIBUTE_TILE_SIZE).read(blockSize_);

    levels_ = _volGroup.getGroup(BBIC_GROUP_LEVELS).getNumberObjects();

    register_lzf();
}

std::array<size_t, 3> File::getBlockCount(const uint32_t level) const
{
    // Alternatively, this information could be read from the bbic level
    // groups
    return {
        {static_cast<size_t>(std::ceil(float(width_ >> level) / blockSize_)),
         static_cast<size_t>(std::ceil(float(height_ >> level) / blockSize_)),
         static_cast<size_t>(std::ceil(float(depth_ >> level) / blockSize_))}};
}

std::vector<std::array<uint8_t, 3>> File::getData(
    const uint32_t level, const std::array<uint32_t, 3>& blockIndex) const
{
    std::stringstream path;
    path << BBIC_GROUP_LEVELS << "/" << level << "/" << blockIndex[0] << "/"
         << blockIndex[1] << "/" << blockIndex[2];

    std::vector<std::array<uint8_t, 3>> data;

#ifndef H5_HAVE_THREADSAFE
    std::lock_guard<std::mutex> lock(h5mutex_);
#endif

    HighFive::SilenceHDF5 silence;

    const auto dataset = _volGroup.getDataSet(path.str());

    const auto space = dataset.getSpace();

    const auto dim = space.getDimensions();
    data.resize(dim[0] * dim[1] * dim[2]);

    const hsize_t memdims[] = {dim[0], dim[1], dim[2]};
    const hid_t memspace = H5Screate_simple(3, memdims, 0);
    H5Dread(dataset.getId(), H5T_NATIVE_UINT8, memspace, space.getId(),
            H5P_DEFAULT, data.data());
    H5Sclose(memspace);
    return data;
}

brayns::Boxd File::getBoundingBox() const
{
    brayns::Boxd bbox;
    bbox.merge({{0, 0, 0},
                {float(getWidth()), float(getHeight()), float(getDepth())}});
    return bbox;
}
}
