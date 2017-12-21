/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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

#ifndef ABSTRACTSIMULATIONHANDLER_H
#define ABSTRACTSIMULATIONHANDLER_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{
/**
 * @brief The AbstractSimulationHandler class handles simulation frames for the
 * current circuit
 */
class AbstractSimulationHandler
{
public:
    /**
     * @brief Default contructor
     * @param geometryParameters Geometry parameters
     */
    AbstractSimulationHandler(const GeometryParameters& geometryParameters);

    /**
     * @brief Default desctuctor
     */
    virtual ~AbstractSimulationHandler();

    /**
    * @brief Attaches a memory mapped file to the scene so that renderers can
    * access the data
    *        as if it was in memory. The OS is in charge of dealing with the map
    * file in system
    *        memory.
    * @param cacheFile File containing the simulation values
    * @return True if the file was successfully attached, false otherwise
    */
    BRAYNS_API bool attachSimulationToCacheFile(const std::string& cacheFile);

    /**
    * @brief Writes the header to a stream. The header contains the number of
    * frames and the frame
    *        size.
    * @param stream Stream where the header should be written
    */
    BRAYNS_API void writeHeader(std::ofstream& stream);

    /**
    * @brief Writes a frame to a stream. A frame is a set of float values.
    * @param stream Stream where the header should be written
    * @param values Frame values
    */
    BRAYNS_API void writeFrame(std::ofstream& stream, const floats& values);

    /** @return the current loaded frame for the simulation. */
    uint32_t getCurrentFrame() const { return _currentFrame; }
    /**
     * @brief returns a void pointer to the simulation data for the given frame
     * or nullptr if the frame is not loaded yet.
     */
    virtual void* getFrameData(uint32_t frame) = 0;

    /**
     * @brief getFrameSize return the size of the current simulation frame
     */
    uint64_t getFrameSize() const { return _frameSize; }
    /**
     * @brief setFrameSize Sets the size of the current simulation frame
     */
    void setFrameSize(const uint64_t frameSize) { _frameSize = frameSize; }
    /**
     * @brief getNbFrames returns the number of frame for the current simulation
     */
    uint32_t getNbFrames() const { return _nbFrames; }
    /**
     * @brief setNbFrames sets the number of frame for the current simulation
     */
    void setNbFrames(const uint32_t nbFrames) { _nbFrames = nbFrames; }
    /**
     * @brief getHistogram returns the Histogram of the values in the current
     * simulation frame. The
     *        size of the histogram is defined by the
     * --simulation-histogram-size command line
     *        parameter (128 by default). To build the histogram, occurrences of
     * the same value are
     *        counted and spread along the histogram according to the calculated
     * range. The
     *        range is defined by the minimum and maximum value of the current
     * frame. The Histogram
     *        is specific to the current frame, not to the whole simulation.
     */
    Histogram& getHistogram();

    /** @return true if the histogram has changed since the last update. */
    bool histogramChanged() const;

    /** @return true if the requested frame from getFrameData() is ready to
     * consume and if it is allowed to advance to the next frame. */
    virtual bool isReady() const { return true; }
protected:
    uint32_t _getBoundedFrame(const uint32_t frame) const;

    const GeometryParameters& _geometryParameters;
    uint32_t _currentFrame;
    uint32_t _nbFrames;
    uint64_t _frameSize;

    uint64_t _headerSize;
    void* _memoryMapPtr;
    int _cacheFileDescriptor;
    Histogram _histogram;
    float* _frameData;
};
}
#endif // ABSTRACTSIMULATIONHANDLER_H
