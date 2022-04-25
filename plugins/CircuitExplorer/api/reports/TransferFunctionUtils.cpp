/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "TransferFunctionUtils.h"

brayns::TransferFunction TransferFunctionUtils::createUnipolarTransferFunction() noexcept
{
    brayns::TransferFunction tf;

    tf.setControlPoints({{0.f, 1.f}, {1.f, 1.f}});
    tf.setColors(
        {{0.0, 0.0, 0.0},
         {0.00392156862745098, 0.00392156862745098, 0.12941176470588237},
         {0.00784313725490196, 0.00784313725490196, 0.25882352941176473},
         {0.011764705882352941, 0.011764705882352941, 0.39215686274509803},
         {0.01568627450980392, 0.01568627450980392, 0.5215686274509804},
         {0.0196078431372549, 0.0196078431372549, 0.6549019607843137},
         {0.03529411764705882, 0.0784313725490196, 0.6862745098039216},
         {0.047058823529411764, 0.13333333333333333, 0.7215686274509804},
         {0.058823529411764705, 0.18823529411764706, 0.7568627450980392},
         {0.07450980392156863, 0.24705882352941178, 0.788235294117647},
         {0.08627450980392157, 0.30196078431372547, 0.8235294117647058},
         {0.09803921568627451, 0.3607843137254902, 0.8588235294117647},
         {0.11372549019607843, 0.41568627450980394, 0.8901960784313725},
         {0.12549019607843137, 0.47058823529411764, 0.9254901960784314},
         {0.13725490196078433, 0.5294117647058824, 0.9568627450980393},
         {0.2196078431372549, 0.4666666666666667, 0.8745098039215686},
         {0.30196078431372547, 0.403921568627451, 0.796078431372549},
         {0.3843137254901961, 0.3411764705882353, 0.7137254901960784},
         {0.4823529411764706, 0.28627450980392155, 0.596078431372549},
         {0.5764705882352941, 0.22745098039215686, 0.47843137254901963},
         {0.6705882352941176, 0.16862745098039217, 0.36470588235294116},
         {0.7686274509803922, 0.11372549019607843, 0.24705882352941178},
         {0.8627450980392157, 0.054901960784313725, 0.13333333333333333},
         {0.9568627450980393, 0.0, 0.01568627450980392},
         {0.9568627450980393, 0.0196078431372549, 0.01568627450980392},
         {0.9529411764705882, 0.043137254901960784, 0.01568627450980392},
         {0.9490196078431372, 0.06666666666666667, 0.01568627450980392},
         {0.9450980392156862, 0.08627450980392157, 0.01568627450980392},
         {0.9411764705882353, 0.10980392156862745, 0.01568627450980392},
         {0.9372549019607843, 0.13333333333333333, 0.011764705882352941},
         {0.9333333333333333, 0.1568627450980392, 0.011764705882352941},
         {0.9333333333333333, 0.17647058823529413, 0.011764705882352941},
         {0.9294117647058824, 0.2, 0.011764705882352941},
         {0.9254901960784314, 0.2235294117647059, 0.011764705882352941},
         {0.9215686274509803, 0.24705882352941178, 0.011764705882352941},
         {0.9176470588235294, 0.26666666666666666, 0.00784313725490196},
         {0.9137254901960784, 0.2901960784313726, 0.00784313725490196},
         {0.9098039215686274, 0.3137254901960784, 0.00784313725490196},
         {0.9098039215686274, 0.33725490196078434, 0.00784313725490196},
         {0.9058823529411765, 0.3568627450980392, 0.00784313725490196},
         {0.9019607843137255, 0.3803921568627451, 0.00784313725490196},
         {0.8980392156862745, 0.403921568627451, 0.00392156862745098},
         {0.8941176470588236, 0.4235294117647059, 0.00392156862745098},
         {0.8901960784313725, 0.4470588235294118, 0.00392156862745098},
         {0.8862745098039215, 0.47058823529411764, 0.00392156862745098},
         {0.8823529411764706, 0.49411764705882355, 0.00392156862745098},
         {0.8823529411764706, 0.5137254901960784, 0.00392156862745098},
         {0.8784313725490196, 0.5372549019607843, 0.0},
         {0.8745098039215686, 0.5607843137254902, 0.0},
         {0.8705882352941177, 0.5843137254901961, 0.0},
         {0.8666666666666667, 0.6039215686274509, 0.0},
         {0.8627450980392157, 0.6274509803921569, 0.0},
         {0.8588235294117647, 0.6509803921568628, 0.0},
         {0.8588235294117647, 0.6745098039215687, 0.0},
         {0.8588235294117647, 0.6823529411764706, 0.01568627450980392},
         {0.8627450980392157, 0.6901960784313725, 0.03529411764705882},
         {0.8666666666666667, 0.7019607843137254, 0.050980392156862744},
         {0.8705882352941177, 0.7098039215686275, 0.07058823529411765},
         {0.8705882352941177, 0.7176470588235294, 0.08627450980392157},
         {0.8745098039215686, 0.7294117647058823, 0.10588235294117647},
         {0.8784313725490196, 0.7372549019607844, 0.12549019607843137},
         {0.8823529411764706, 0.7450980392156863, 0.1411764705882353},
         {0.8823529411764706, 0.7568627450980392, 0.1607843137254902},
         {0.8862745098039215, 0.7647058823529411, 0.17647058823529413},
         {0.8901960784313725, 0.7764705882352941, 0.19607843137254902},
         {0.8941176470588236, 0.7843137254901961, 0.21568627450980393},
         {0.8980392156862745, 0.792156862745098, 0.23137254901960785},
         {0.8980392156862745, 0.803921568627451, 0.25098039215686274},
         {0.9019607843137255, 0.8117647058823529, 0.26666666666666666},
         {0.9058823529411765, 0.8196078431372549, 0.28627450980392155},
         {0.9098039215686274, 0.8313725490196079, 0.3058823529411765},
         {0.9098039215686274, 0.8392156862745098, 0.3215686274509804},
         {0.9137254901960784, 0.8509803921568627, 0.3411764705882353},
         {0.9176470588235294, 0.8588235294117647, 0.3568627450980392},
         {0.9215686274509803, 0.8666666666666667, 0.3764705882352941},
         {0.9215686274509803, 0.8784313725490196, 0.396078431372549},
         {0.9254901960784314, 0.8862745098039215, 0.4117647058823529},
         {0.9294117647058824, 0.8941176470588236, 0.43137254901960786},
         {0.9333333333333333, 0.9058823529411765, 0.4470588235294118},
         {0.9372549019607843, 0.9137254901960784, 0.4666666666666667},
         {0.9372549019607843, 0.9254901960784314, 0.48627450980392156},
         {0.9411764705882353, 0.9333333333333333, 0.5019607843137255},
         {0.9450980392156862, 0.9411764705882353, 0.5215686274509804},
         {0.9490196078431372, 0.9529411764705882, 0.5372549019607843},
         {0.9490196078431372, 0.9607843137254902, 0.5568627450980392},
         {0.9529411764705882, 0.9686274509803922, 0.5764705882352941},
         {0.9568627450980393, 0.9803921568627451, 0.592156862745098},
         {0.9607843137254902, 0.9882352941176471, 0.611764705882353},
         {0.9647058823529412, 1.0, 0.6274509803921569},
         {0.9647058823529412, 1.0, 0.6392156862745098},
         {0.9647058823529412, 1.0, 0.6470588235294118},
         {0.9647058823529412, 1.0, 0.6588235294117647},
         {0.9647058823529412, 1.0, 0.6666666666666666},
         {0.9686274509803922, 1.0, 0.6745098039215687},
         {0.9686274509803922, 1.0, 0.6862745098039216},
         {0.9686274509803922, 1.0, 0.6941176470588235},
         {0.9686274509803922, 1.0, 0.7019607843137254},
         {0.9725490196078431, 1.0, 0.7137254901960784},
         {0.9725490196078431, 1.0, 0.7215686274509804},
         {0.9725490196078431, 1.0, 0.7294117647058823},
         {0.9725490196078431, 1.0, 0.7411764705882353},
         {0.9725490196078431, 1.0, 0.7490196078431373},
         {0.9764705882352941, 1.0, 0.7568627450980392},
         {0.9764705882352941, 1.0, 0.7686274509803922},
         {0.9764705882352941, 1.0, 0.7764705882352941},
         {0.9764705882352941, 1.0, 0.7843137254901961},
         {0.9803921568627451, 1.0, 0.796078431372549},
         {0.9803921568627451, 1.0, 0.803921568627451},
         {0.9803921568627451, 1.0, 0.8117647058823529},
         {0.9803921568627451, 1.0, 0.8235294117647058},
         {0.9803921568627451, 1.0, 0.8313725490196079},
         {0.984313725490196, 1.0, 0.8431372549019608},
         {0.984313725490196, 1.0, 0.8509803921568627},
         {0.984313725490196, 1.0, 0.8588235294117647},
         {0.984313725490196, 1.0, 0.8705882352941177},
         {0.9882352941176471, 1.0, 0.8784313725490196},
         {0.9882352941176471, 1.0, 0.8862745098039215},
         {0.9882352941176471, 1.0, 0.8980392156862745},
         {0.9882352941176471, 1.0, 0.9058823529411765},
         {0.9882352941176471, 1.0, 0.9137254901960784},
         {0.9921568627450981, 1.0, 0.9254901960784314},
         {0.9921568627450981, 1.0, 0.9333333333333333},
         {0.9921568627450981, 1.0, 0.9411764705882353},
         {0.9921568627450981, 1.0, 0.9529411764705882},
         {0.996078431372549, 1.0, 0.9607843137254902},
         {0.996078431372549, 1.0, 0.9686274509803922},
         {0.996078431372549, 1.0, 0.9803921568627451},
         {1.0, 1.0, 1.0}});
    tf.setValuesRange({-80, -10});

    return tf;
}

brayns::OSPBuffer TransferFunctionUtils::createSampleBuffer(const brayns::TransferFunction &tf) noexcept
{
    std::vector<brayns::Vector4f> colors(256);

    const auto &range = tf.getValuesRange();
    const auto chunk = (range.y - range.x) / 256.f;

    for (size_t i = 0; i < 256; ++i)
    {
        const auto value = range.x + chunk * i;
        auto color = tf.getColorForValue(value);
        colors[i] = std::move(color);
    }

    return brayns::DataHandler::copyBuffer(colors, OSPDataType::OSP_VEC4F);
}
