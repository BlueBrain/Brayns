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

#pragma once

#include <brayns/common/types.h>
#include <highfive/H5File.hpp>

#include <array>
#include <vector>
#ifndef H5_HAVE_THREADSAFE
#include <mutex>
#endif

namespace bbic
{
class File
{
public:
    explicit File(const std::string& file);

    std::array<size_t, 3> getBlockCount(const uint32_t level) const;

    std::vector<std::array<uint8_t, 3>> getData(
        const uint32_t level, const std::array<uint32_t, 3>& blockIndex) const;

    size_t getBlockSize() const { return blockSize_; }
    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }
    size_t getDepth() const { return depth_; }
    size_t getLevels() const { return levels_; }
    brayns::Boxd getBoundingBox() const;

private:
    std::unique_ptr<HighFive::File> _file;
    const HighFive::Group _volGroup;

    size_t width_;
    size_t height_;
    size_t depth_;
    size_t levels_;
    size_t blockSize_;

#ifndef H5_HAVE_THREADSAFE
    // Every access to hdf5 must be serialized if HDF5 does not take care of it
    // which needs a thread-safe built of the library.
    // http://www.hdfgroup.org/hdf5-quest.html#gconc
    mutable std::mutex h5mutex_;
#endif
};
} // namespace bbic
