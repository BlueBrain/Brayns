/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

namespace brayns
{
/**
 * @brief Used to monitor the image stream.
 *
 */
class StreamMonitor
{
public:
    /**
     * @brief Check if the stream is controlled.
     *
     * If true, the client trigger the stream when an image is required.
     *
     * @return true Controlled.
     * @return false Automatic.
     */
    bool isControlled() const;

    /**
     * @brief Set the stream control mode.
     *
     * @param controlled True if controlled, False if automatic.
     */
    void setControlled(bool controlled);

    /**
     * @brief Check if the image stream has been triggered.
     *
     * @return true Client triggered the image stream.
     * @return false Nothing triggered.
     */
    bool isTriggered() const;

    /**
     * @brief Trigger the image stream in controlled mode.
     *
     */
    void trigger();

    /**
     * @brief Reset the trigger in control mode.
     *
     */
    void resetTrigger();

private:
    bool _controlled = false;
    bool _triggered = false;
};
} // namespace brayns
