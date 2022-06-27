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

#include <brayns/engine/imageoperations/ImageOperation.h>

namespace brayns
{
class ToneMapping final : public ImageOperation
{
public:
    ToneMapping();

    void commit() override;

    /**
     * @brief Sets the amount of light per unit area
     * @param exposure
     */
    void setExposure(const float exposure) noexcept;

    /**
     * @brief Returns the amount of light per unit area
     * @return
     */
    float getExposure() const noexcept;

    /**
     * @brief Sets the contrast
     * @param contrast
     */
    void setContrast(const float contrast) noexcept;

    /**
     * @brief Returns the contrast
     * @return float
     */
    float getContrast() const noexcept;

    /**
     * @brief Sets the highlight compression
     * @param shoulder
     */
    void setShoulder(const float shoulder) noexcept;

    /**
     * @brief Returns the highlight compression
     * @return float
     */
    float getShoulder() const noexcept;

    /**
     * @brief Sets the mid-level anchor input
     * @param midin
     */
    void setMidIn(const float midin) noexcept;

    /**
     * @brief Returns the mid-level anchor input
     * @return float
     */
    float getMidIn() const noexcept;

    /**
     * @brief Sets the mid-level anchor output
     * @param midout
     */
    void setMidOut(const float midout) noexcept;

    /**
     * @brief Returns the mid-level anchor output
     * @return float
     */
    float getMidOut() const noexcept;

    /**
     * @brief Sets the maximum HDR input that is not clipped
     * @param maxHDR
     */
    void setMaxHDR(const float maxHDR) noexcept;

    /**
     * @brief Returns the maximum HDR input that is not clipped
     * @return float
     */
    float getMaxHDR() const noexcept;

    /**
     * @brief Sets wether to use ACES color transformations or not
     * @param val
     */
    void useACESColor(const bool val) noexcept;

    /**
     * @brief Returns wether the ACES color transformations are being used or not
     * @return bool
     */
    bool usesACESColor() const noexcept;

private:
    float _exposure{1.f};
    // Uncharted 2 // ACES
    float _contrast{1.1759f}; //{1.6773f};
    float _shoulder{0.9746f}; //{0.9714f};
    float _midIn{0.18f}; //{0.18f};
    float _midOut{0.18f}; //{0.18f};
    float _hdrMax{6.3704f}; //{11.0785f};
    bool _acesColor{false}; //{true};
};
}
