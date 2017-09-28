/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <functional>
#include <memory>
#include <string>

namespace brayns
{
/**
 * A class which tracks and reports progress of an operation/execution.
 * It reports on stdout using boost::progress_display and also reports to a
 * user-defined callback.
 *
 * It is safe to use in threaded environments like OpenMP for-loops. In that
 * case, only the first OpenMP thread reports progress, but still tracks from
 * all threads.
 */
class Progress
{
public:
    /**
     * The callback for each progress update with the signature (message,
     * current count, expected count)
     */
    using ProgressUpdateCallback =
        std::function<void(const std::string&, size_t, size_t)>;

    /**
     * Setup the progress and already report progress '0' on the given callback.
     * @param message the current operation that takes place
     * @param expectedCount the expected count of updates when the reporting
     *        should finish
     * @param updateCallback a user-defined callback to be called on each
     *        update.
     */
    Progress(const std::string& message, unsigned long expectedCount,
             const ProgressUpdateCallback& updateCallback =
                 ProgressUpdateCallback());

    /**
     * Reports 'expected count' on the callback to ensure completion of the
     * progress reporting.
     */
    ~Progress();

    Progress(Progress&& rhs);
    Progress& operator=(Progress&& rhs);

    /**
     * Update the message for a running progress. Only supported for the
     * callback, the stdout printing is unaffected.
     */
    void setMessage(const std::string& message);

    /**
     * Increment the progress by one, which may report to stdout, but always to
     * the callback.
     */
    void operator++();

    /**
     * Increment the progress by any value, which may report to stdout, but
     * always to the callback.
     */
    void operator+=(size_t increment);

    /** @return the current progress count. */
    size_t getCurrent() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
}
