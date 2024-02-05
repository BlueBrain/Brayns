/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "ColorUtils.h"

ColorTable::ColorTable()
{
    VALUES = {
        {1.0, 0.9490196078431372, 0.0, 1.0},
        {0.9686274509803922, 0.5803921568627451, 0.11372549019607843, 1.0},
        {0.8784313725490196, 0.1843137254901961, 0.3803921568627451, 1.0},
        {0.9882352941176471, 0.6078431372549019, 0.9921568627450981, 1.0},
        {0.40784313725490196, 0.6588235294117647, 0.8784313725490196, 1.0},
        {0.4235294117647059, 0.9019607843137255, 0.3843137254901961, 1.0},
        {0.6509803921568628, 0.33725490196078434, 0.1568627450980392, 1.0},
        {0.9686274509803922, 0.5058823529411764, 0.7490196078431373, 1.0},
        {0.6, 0.6, 0.6, 1.0},
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745, 1.0},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804, 1.0},
        {0.30196078431372547, 0.6862745098039216, 0.2901960784313726, 1.0},
        {0.596078431372549, 0.3058823529411765, 0.6392156862745098, 1.0},
        {1.0, 0.4980392156862745, 0.0, 1.0},
        {1.0, 1.0, 0.2, 1.0},
        {0.6509803921568628, 0.33725490196078434, 0.1568627450980392, 1.0},
        {0.9686274509803922, 0.5058823529411764, 0.7490196078431373, 1.0},
        {0.6, 0.6, 0.6, 1.0},
        {0.8941176470588236, 0.10196078431372549, 0.10980392156862745, 1.0},
        {0.21568627450980393, 0.49411764705882355, 0.7215686274509804, 1.0}};
}

ColorTable ColorDeck::_TABLE;

const brayns::Vector4f& ColorDeck::getColorForKey(const std::string& k) noexcept
{
    const auto it = _colorMap.find(k);
    if (it == _colorMap.end())
        return _emplaceColor(k);

    return _TABLE.VALUES[it->second];
}

const brayns::Vector4f& ColorDeck::_emplaceColor(const std::string& k) noexcept
{
    const auto nextIndex = _lastIndex++ % _TABLE.VALUES.size();
    _colorMap[k] = nextIndex;
    return _TABLE.VALUES[nextIndex];
}

ColorTable ColorRoulette::_TABLE;

const brayns::Vector4f& ColorRoulette::getNextColor() noexcept
{
    const auto index = _lastIndex++ % _TABLE.VALUES.size();
    return _TABLE.VALUES[index];
}
