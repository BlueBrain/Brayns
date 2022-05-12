/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/ImageOperation.h>
#include <brayns/utils/IDFactory.h>

#include <memory>

namespace brayns
{
class ImageOperationManager
{
public:
    ImageOperationManager();

    /**
     * @brief Adds a new operation into the manager. Returns the operation ID
     * @param operation
     * @return uint32_t
     */
    uint32_t addOperation(std::unique_ptr<ImageOperation> operation) noexcept;

    /**
     * @brief Removes an operation from the manager
     * @param id ID of the operation
     * @throws std::invalid_argument if the id does not correspond to an existing opration
     */
    void removeOperation(const uint32_t id);

private:
    friend class FrameBuffer;

    /**
     * @brief Returns a list of handles to the operations
     * @return std::vector<OSPImageOperation>
     */
    std::vector<OSPImageOperation> getOperationHandles() const noexcept;

private:
    struct OperationEntry
    {
        uint32_t id;
        std::unique_ptr<ImageOperation> operation;
    };

    IDFactory<uint32_t> _idFactory;
    std::vector<OperationEntry> _operations;
};
}
