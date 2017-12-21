/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include "VolumeHandler.h"

#include <brayns/common/log.h>

#include <fcntl.h>
#include <fstream>
#include <future>
#include <sys/mman.h>
#include <sys/stat.h>

namespace
{
const int NO_DESCRIPTOR = -1;
}

namespace brayns
{
VolumeHandler::VolumeHandler(const VolumeParameters& volumeParameters,
                             const IndexMode indexMode)
    : _volumeParameters(volumeParameters)
    , _currentIndex(std::numeric_limits<uint32_t>::max())
    , _indexMode(indexMode)
{
}

VolumeHandler::~VolumeHandler()
{
    _volumeDescriptors.clear();
}

void VolumeHandler::attachVolumeToFile(const uint32_t index,
                                       const std::string& volumeFile)
{
    // Add volume descriptor for specified index
    _volumeDescriptors[index].reset(
        new VolumeDescriptor(volumeFile, _volumeParameters.getDimensions(),
                             _volumeParameters.getElementSpacing(),
                             _volumeParameters.getOffset()));

    BRAYNS_INFO << "Attached " << volumeFile << " to index " << index << " ["
                << _volumeDescriptors.begin()->first << ", "
                << _volumeDescriptors.rbegin()->first << "]" << std::endl;
}

void VolumeHandler::setCurrentIndex(uint32_t index)
{
    index = _getBoundedIndex(index);
    if (index != _currentIndex &&
        _volumeDescriptors.find(index) != _volumeDescriptors.end())
    {
        if (_volumeDescriptors.find(_currentIndex) != _volumeDescriptors.end())
            _volumeDescriptors[_currentIndex]->unmap();
        _currentIndex = index;
        _volumeDescriptors[_currentIndex]->map();
    }
}

void* VolumeHandler::getData() const
{
    if (_volumeDescriptors.find(_currentIndex) != _volumeDescriptors.end())
        return _volumeDescriptors.at(_currentIndex)->getMemoryMapPtr();
    return nullptr;
}

float VolumeHandler::getEpsilon(const Vector3f& elementSpacing,
                                const uint16_t samplesPerRay)
{
    if (_volumeDescriptors.find(_currentIndex) == _volumeDescriptors.end())
        return 0.f;
    const Vector3f diag =
        elementSpacing * _volumeDescriptors.at(_currentIndex)->getDimensions();
    const float diagMax = diag.find_max();
    const float epsilon = diagMax / float(samplesPerRay);
    return std::max(1.f, epsilon);
}

Vector3ui VolumeHandler::getDimensions() const
{
    if (_volumeDescriptors.find(_currentIndex) != _volumeDescriptors.end())
        return _volumeDescriptors.at(_currentIndex)->getDimensions();
    return Vector3ui();
}

Vector3f VolumeHandler::getElementSpacing() const
{
    if (_volumeDescriptors.find(_currentIndex) != _volumeDescriptors.end())
        return _volumeDescriptors.at(_currentIndex)->getElementSpacing();
    return Vector3f();
}

Vector3f VolumeHandler::getOffset() const
{
    if (_volumeDescriptors.find(_currentIndex) != _volumeDescriptors.end())
        return _volumeDescriptors.at(_currentIndex)->getOffset();
    return Vector3f();
}

uint64_t VolumeHandler::getSize() const
{
    if (_volumeDescriptors.find(_currentIndex) != _volumeDescriptors.end())
        return _volumeDescriptors.at(_currentIndex)->getSize();
    return 0;
}

uint32_t VolumeHandler::_getBoundedIndex(const uint32_t index) const
{
    uint32_t result = 0;
    switch (_indexMode)
    {
    case IndexMode::modulo:
        if (_volumeDescriptors.size() != 0)
            result = size_t(index + _volumeDescriptors.begin()->first) %
                     _volumeDescriptors.size();
        break;
    case IndexMode::bounded:
        result = std::max(std::min(index, _volumeDescriptors.rbegin()->first),
                          _volumeDescriptors.begin()->first);
    case IndexMode::unchanged:
    default:
        result = index;
    }
    return result;
}

VolumeHandler::VolumeDescriptor::VolumeDescriptor(
    const std::string& filename, const Vector3ui& dimensions,
    const Vector3f& elementSpacing, const Vector3f& offset)
    : _filename(filename)
    , _memoryMapPtr(0)
    , _cacheFileDescriptor(NO_DESCRIPTOR)
    , _dimensions(dimensions)
    , _elementSpacing(elementSpacing)
    , _offset(offset)
{
}

VolumeHandler::VolumeDescriptor::~VolumeDescriptor()
{
    unmap();
}

void VolumeHandler::VolumeDescriptor::map()
{
    _cacheFileDescriptor = open(_filename.c_str(), O_RDONLY);
    if (_cacheFileDescriptor == NO_DESCRIPTOR)
    {
        BRAYNS_ERROR << "Failed to attach " << _filename << std::endl;
        return;
    }

    struct stat sb;
    if (::fstat(_cacheFileDescriptor, &sb) == NO_DESCRIPTOR)
    {
        BRAYNS_ERROR << "Failed to attach " << _filename << std::endl;
        return;
    }

    _size = sb.st_size;
    _memoryMapPtr =
        ::mmap(0, _size, PROT_READ, MAP_PRIVATE, _cacheFileDescriptor, 0);
    if (_memoryMapPtr == MAP_FAILED)
    {
        _memoryMapPtr = 0;
        ::close(_cacheFileDescriptor);
        _cacheFileDescriptor = NO_DESCRIPTOR;
        BRAYNS_ERROR << "Failed to attach " << _filename << std::endl;
        return;
    }
}

void VolumeHandler::VolumeDescriptor::unmap()
{
    if (_memoryMapPtr)
    {
        ::munmap((void*)_memoryMapPtr, _size);
        _memoryMapPtr = 0;
    }
    if (_cacheFileDescriptor != NO_DESCRIPTOR)
    {
        ::close(_cacheFileDescriptor);
        _cacheFileDescriptor = NO_DESCRIPTOR;
    }
}

Histogram& VolumeHandler::getHistogram()
{
    if (_histograms.find(_currentIndex) != _histograms.end())
        return _histograms[_currentIndex];

    std::future<bool> computeHistogram =
        std::async(std::launch::async, [this]() {
            uint8_t* data = static_cast<uint8_t*>(getData());
            if (data)
            {
                BRAYNS_INFO << "Computing volume histogram" << std::endl;
                uint8_t minValue = std::numeric_limits<uint8_t>::max();
                uint8_t maxValue = 0;
                std::map<uint8_t, uint64_t> values;
                for (uint64_t i = 0; i < getSize(); ++i)
                {
                    const uint8_t value = data[i];
                    minValue = std::min(minValue, value);
                    maxValue = std::max(maxValue, value);
                    if (values.find(value) == values.end())
                        values[value] = 1;
                    else
                        ++values[value];
                }

                _histograms[_currentIndex].values.clear();
                for (const auto& value : values)
                    _histograms[_currentIndex].values.push_back(value.second);
                _histograms[_currentIndex].range = Vector2f(minValue, maxValue);
                BRAYNS_INFO
                    << "Histogram range: " << _histograms[_currentIndex].range
                    << std::endl;
            }
            return true;
        });

    computeHistogram.wait();
    computeHistogram.get();

    return _histograms[_currentIndex];
}
}
