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

#ifndef VOLUMEHANDLER_H
#define VOLUMEHANDLER_H

#include <brayns/common/types.h>
#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
/**
   VolumeHandler object

   This object contains handle to one or several 8bit volumes. Files containing
   volumes are accessed via memory maps and each volume is assigned to a given
   index.
 */
class VolumeHandler
{
public:
    /**
     * @brief Default constructor
     * @param volumeParameters Parameters for the volume (Offset, Scaling, Space
     *        between elements)
     * @param indexMode Specifies the way the index in handled:
     *        - DEFAULT: Gets the volume corresponding to the index
     *        - MODULO : Gets the volume using the index modulo the number of
     *                   volumes attached to the handler
     *        - BOUNDED: The index is bounded by the index range for the
     *                   attached volumes
     */
    VolumeHandler(const VolumeParameters& volumeParameters,
                  IndexMode indexMode);

    ~VolumeHandler();

    /**
     * @brief Returns the dimension of the 8bit volume
     * @return Dimensions of the volume
     */
    Vector3ui getDimensions() const;

    /**
     * @brief Returns the voxel size of the 8bit volume
     * @return Voxel size of the volume
     */
    Vector3f getElementSpacing() const;

    /**
     * @brief Returns the position offset of the 8bit volume in world
     *        coordinates
     * @return Volume offset position
     */
    Vector3f getOffset() const;

    /**
     * @brief Returns the size of the 8bit volume in bytes
     * @return Size of the volume
     */
    uint64_t getSize() const;

    /**
     * @brief Returns a pointer to a given frame in the memory mapped file.
     * @return Pointer to volume
     */
    void* getData() const;

    /**
     * @brief Returns the epsilon that defines the step used to walk along the
     *        ray when traversing the volume. The value is defined according to
     *        the dimensions and scaling of the volume, and the number of
     *        samples per ray
     * @param elementSpacing Scaling between elements of the volume. This
     *        attribute represents the voxel size
     * @param samplesPerRay Number of samples per ray
     * @return The epsilon
     */
    float getEpsilon(const Vector3f& elementSpacing,
                     const uint16_t samplesPerRay);

    /**
    * @brief Attaches a memory mapped file to the scene so that renderers can
    *        access the data as if it was in memory. The OS is in charge of
    *        dealing with the map file in system memory.
    * @param index Index for the volume
    * @param volumeFile File containing the 8bit volume
    * @return True if the file was successfully attached, false otherwise
    */
    void attachVolumeToFile(const uint32_t index,
                            const std::string& volumeFile);

    /**
     * @brief Sets the index mode
     */
    void setIndexMode(const IndexMode mode) { _indexMode = mode; }
    /**
     * @brief Gets the index mode
     */
    IndexMode getIndexMode() const { return _indexMode; }
    /**
     * @brief Sets the index for the volume handler. If the specified index is
     *        different from the current one, the current volume is unmapped and
     *        the new one is mapped instead.
     * @param index Index for the volume
     */
    void setCurrentIndex(const uint32_t index);

    /** Set the histogram of the currently loaded volume. */
    void setHistogram(const Histogram& histogram)
    {
        _histograms[_currentIndex] = histogram;
    }
    /** @return the histogram of the currently loaded volume. */
    Histogram& getHistogram();
    /** @return the number of frames of the current volume. */
    uint64_t getNbFrames() const { return _nbFrames; }
    /** Sets the number of frames for the current volume. */
    void setNbFrames(const uint64_t nbFrames) { _nbFrames = nbFrames; }
    /**
     * @brief The VolumeDescriptor class handles the attribute of a single
     *        volume. The class is in charge of the mapping to the volume file
     */
    class VolumeDescriptor
    {
    public:
        VolumeDescriptor(const std::string& filename,
                         const Vector3ui& dimensions,
                         const Vector3f& elementSpacing,
                         const Vector3f& offset);
        ~VolumeDescriptor();

        /**
         * @brief Maps the volume to the corresponding _filename
         */
        void map();

        /**
         * @brief Unmaps the volume from the corresponding _filename
         */
        void unmap();

        /**
         * @brief Returns the file descriptor for the current volume
         * @return File descriptor for the current volume
         */
        int getCacheFileDescriptor() const { return _cacheFileDescriptor; }
        /**
         * @brief Returns the size of the volume in bytes
         * @return Size of the volume in bytes
         */
        uint64_t getSize() const { return _size; }
        /**
         * @brief Returns a pointer to volume file
         * @return Pointer to volume file
         */
        void* getMemoryMapPtr() const { return _memoryMapPtr; }
        /**
         * @brief Returns the dimensions of the volume
         * @return Dimensions of the volume
         */
        Vector3ui getDimensions() const { return _dimensions; }
        /**
         * @brief Returns the voxel size of the volume
         * @return Voxel size of the volume
         */
        Vector3f getElementSpacing() const { return _elementSpacing; }
        /**
         * @brief Returns the volume position offset
         * @return Volume position offset
         */
        Vector3f getOffset() const { return _offset; }
        /**
         * @brief Returns the volume filename
         * @return Filename of the volume
         */
        const std::string& getFilename() const { return _filename; }
    private:
        std::string _filename;
        void* _memoryMapPtr;
        int _cacheFileDescriptor;
        uint64_t _size;
        Vector3ui _dimensions;
        Vector3f _elementSpacing;
        Vector3f _offset;
    };
    typedef std::shared_ptr<VolumeDescriptor> VolumeDescriptorPtr;

private:
    uint32_t _getBoundedIndex(const uint32_t index) const;

    const VolumeParameters& _volumeParameters;
    std::map<uint32_t, VolumeDescriptorPtr> _volumeDescriptors;
    uint32_t _currentIndex;
    IndexMode _indexMode;
    std::map<float, Histogram> _histograms;
    uint64_t _nbFrames = 0;
};
}

#endif // VOLUMEHANDLER_H
