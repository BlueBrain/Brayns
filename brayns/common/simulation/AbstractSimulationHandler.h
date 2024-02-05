/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <limits>
#include <memory>
#include <vector>

namespace brayns
{
/**
 * @brief The AbstractSimulationHandler class handles simulation frames for the
 * current circuit
 */
class AbstractSimulationHandler
{
public:
    AbstractSimulationHandler() = default;
    AbstractSimulationHandler(const AbstractSimulationHandler& o) { *this = o; }

    /** @return a clone of the concrete simulation handler implementation. */
    virtual std::shared_ptr<AbstractSimulationHandler> clone() const = 0;

    virtual ~AbstractSimulationHandler() = default;

    AbstractSimulationHandler& operator=(const AbstractSimulationHandler& rhs);

    /** @return the current loaded frame for the simulation. */
    uint32_t getCurrentFrame() const { return _currentFrame; }

    /**
     * @brief Sets the current frame played by this simulation handler
     * @param newFrame unsigned integer.
     */
    void setCurrentFrame(const uint32_t newFrame) { _currentFrame = newFrame; }

    /**
     * @brief setFrameAdjuster sets the frame adjusting parameter
     *
     * The frame adjuster Multiplies the current frame by this parameter to get
     * the real report frame. Because Brayns now support multiple simulations
     * simultaneously, its possible to have 2 simulations with same start and
     * end time, but different time step (dt). This yields a different number of
     * frames for each report, and without the adjuster, one of them would not
     * be played at the appropiate rate
     *
     * @param adjuster
     */
    void setFrameAdjuster(const double adjuster) { _frameAdjuster = adjuster; }

    /**
     * @brief returns a void pointer to the simulation data for the given frame
     * or nullptr if the frame is not loaded yet.
     */
    void* getFrameData(const uint32_t frame);

    /**
     * @brief Subclasses of the simulation handlers must implement this method
     * to retrieve the data for the given simulation frame
     */
    virtual std::vector<float> getFrameDataImpl(const uint32_t) { return {}; };

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
     * @brief getStartTime Return the time at which the simulation begins
     * @return double
     */
    double getStartTime() const { return _startTime; }

    /**
     * @brief getEndTime Return the time at which the simulation ends
     * @return double
     */
    double getEndTime() const { return _endTime; }

    /**
     * @return the dt of the simulation in getUnit() time unit; 0 if not
     *         reported
     */
    double getDt() const { return _dt; }
    /** @return the time unit of the simulation; empty if not reported. */
    const std::string& getUnit() const { return _unit; }
    /** @return true if the requested frame from getFrameData() is ready to
     * consume and if it is allowed to advance to the next frame. */
    virtual bool isReady() const { return true; }
    /** Wait until current frame is ready */
    virtual void waitReady() const {}

    /**
     * @brief return the frame index within this handler bounds that
     * correspond to the given global frame index
     * @param frame the global frame to be bounded
     */
    uint32_t getBoundedFrame(const uint32_t frame) const;

private:
    uint32_t _currentFrame{std::numeric_limits<uint32_t>::max()};
    std::vector<float> _frameData;

protected:
    uint32_t _nbFrames{0};
    uint64_t _frameSize{0};
    double _startTime{0};
    double _endTime{0};
    double _dt{0};
    // Multiplies the current frame by this parameter to get the real report
    // frame. Because Brayns now support multiple simulations simultaneously,
    // its possible to have 2 simulations with same start and end time, but
    // different time step (dt) This yields a different number of frames for
    // each report, and without the adjuster, One of them would not be played at
    // the appropiate rate frameToLoad = static_cast<uint32_t>(frame *
    // _frameAdjuster)
    double _frameAdjuster{1.0};
    std::string _unit;
};

using AbstractSimulationHandlerPtr = std::shared_ptr<AbstractSimulationHandler>;
} // namespace brayns
