/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "CircuitLoader.h"
#include "ModelData.h"
#include "MorphologyLoader.h"
#include "SimulationHandler.h"

#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/utils.h>

#include <brayns/parameters/ApplicationParameters.h>

#include <brain/brain.h>

#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using namespace brayns;

namespace
{
constexpr size_t LOAD_BATCH_SIZE = 100;
constexpr size_t DEFAULT_SOMA_RADIUS = 5;

using Property = brayns::Property;
const Property PROP_DENSITY = {"density", 100.0, {"Density"}};
const Property PROP_RANDOM_SEED = {"randomSeed", 0, {"Random seed"}};
const Property PROP_REPORT = {"report", std::string(""), {"Report"}};
const Property PROP_TARGETS = {"targets", std::string(""), {"Targets"}};
const Property PROP_MESH_FILENAME_PATTERN = {"meshFilenamePattern",
                                             std::string(""),
                                             {"Mesh filename pattern"}};
const Property PROP_MESH_FOLDER = {"meshFolder",
                                   std::string(""),
                                   {"Mesh folder"}};
const Property PROP_USE_SIMULATION_MODEL = {"useSimulationModel",
                                            false,
                                            {"Use simulation model"}};
const Property PROP_TRANSFORM_MESHES = {"transformMeshes",
                                        false,
                                        {"Transform meshes"}};
const Property PROP_COLOR_SCHEME = {"colorScheme",
                                    brayns::enumToString(ColorScheme::none),
                                    brayns::enumNames<ColorScheme>(),
                                    {"Color scheme"}};
const Property PROP_START_SIMULATION_TIME = {"startSimulationTime",
                                             0.0,
                                             {"Start simulation time"}};
const Property PROP_END_SIMULATION_TIME = {"endSimulationTime",
                                           0.0,
                                           {"End simulation time"}};
const Property PROP_SIMULATION_STEP = {"simulationStep",
                                       0.0,
                                       {"Simulation step"}};
const Property PROP_SYNCHRONOUS_MODE = {"synchronousMode",
                                        false,
                                        {"Synchronous mode"}};
const Property PROP_GEOMETRY_QUALITY = {"geometryQuality",
                                        brayns::enumToString(
                                            GeometryQuality::high),
                                        brayns::enumNames<GeometryQuality>(),
                                        {"Geometry quality"}};
constexpr auto LOADER_NAME = "circuit";
const strings LOADER_EXTENSIONS{"BlueConfig", "BlueConfig3", "CircuitConfig",
                                ".json", "circuit"};

using SimulationHandlerPtr = std::shared_ptr<SimulationHandler>;

#if BRAYNS_USE_ASSIMP
/**
 * @brief getMeshFilenameFromGID Returns the name of the mesh file according
 * to the --circuit-mesh-folder, --circuit-mesh-filename-pattern command
 * line arguments and a GID
 * @param gid GID of the cell
 * @return A string with the full path of the mesh file
 */
std::string _getMeshFilenameFromGID(const uint64_t gid,
                                    const std::string& pattern,
                                    const std::string& folder)
{
    const std::string gidAsString = std::to_string(gid);
    const std::string GID = "{gid}";

    auto filenamePattern = pattern;
    if (!filenamePattern.empty())
        filenamePattern.replace(filenamePattern.find(GID), GID.length(),
                                gidAsString);
    else
        filenamePattern = gidAsString;
    return folder + "/" + filenamePattern;
}
#endif

std::vector<float> _getSomaRadii(const brain::Circuit& circuit,
                                 const float defaultRadius)
{
    std::map<std::string, float> radii;

    // Old nomemclature
    radii["NGC"] = 7.46;
    radii["CRC"] = 7.28;
    radii["BP"] = 6.74;
    radii["L6CTPC"] = 7.00;
    radii["ChC"] = 8.01;
    radii["L4PC"] = 7.84;
    radii["L2PC"] = 8.36;
    radii["LBC"] = 9.28;
    radii["NBC"] = 8.61;
    radii["L5STPC"] = 8.61;
    radii["L6CCPC"] = 7.71;
    radii["L3PC"] = 7.70;
    radii["ADC"] = 7.72;
    radii["L5TTPC"] = 11.17;
    radii["AHC"] = 7.54;
    radii["L5UTPC"] = 9.75;
    radii["MC"] = 9.37;
    radii["DBC"] = 6.58;
    radii["SBC"] = 7.75;
    radii["L4SS"] = 8.87;
    radii["BTC"] = 9.15;
    radii["L4SP"] = 7.76;
    radii["L6CLPC"] = 7.32;

    // New nomemclature
    radii["L1_DAC"] = 7.77242212296;
    radii["L1_NGC-DA"] = 7.71745554606;
    radii["L1_NGC-SA"] = 7.53378756841;
    radii["L1_HAC"] = 7.48774570227;
    radii["L1_DLAC"] = 7.63075244427;
    radii["L1_SLAC"] = 6.54029955183;
    radii["L23_PC"] = 7.7618568695;
    radii["L23_MC"] = 8.25832914484;
    radii["L23_BTC"] = 9.30396906535;
    radii["L23_DBC"] = 6.67491123753;
    radii["L23_BP"] = 6.47212839127;
    radii["L23_NGC"] = 8.40507149696;
    radii["L23_LBC"] = 9.0722948432;
    radii["L23_NBC"] = 8.64244471205;
    radii["L23_SBC"] = 7.77334165573;
    radii["L23_ChC"] = 8.38753128052;
    radii["L4_PC"] = 7.5360350558;
    radii["L4_SP"] = 7.97097187241;
    radii["L4_SS"] = 8.95526583989;
    radii["L4_MC"] = 9.64632482529;
    radii["L4_BTC"] = 8.13213920593;
    radii["L4_DBC"] = 9.06448200771;
    radii["L4_BP"] = 5.80337953568;
    radii["L4_NGC"] = 7.61350679398;
    radii["L4_LBC"] = 8.92902739843;
    radii["L4_NBC"] = 9.02906776877;
    radii["L4_SBC"] = 9.16592723673;
    radii["L4_ChC"] = 8.54107379913;
    radii["L5_TTPC1"] = 12.4378146535;
    radii["L5_TTPC2"] = 12.9319117383;
    radii["L5_UTPC"] = 7.55000627041;
    radii["L5_STPC"] = 8.84197418645;
    radii["L5_MC"] = 9.02451186861;
    radii["L5_BTC"] = 9.24175042372;
    radii["L5_DBC"] = 8.92543775895;
    radii["L5_BP"] = 5.98329114914;
    radii["L5_NGC"] = 6.62666320801;
    radii["L5_LBC"] = 10.0915957915;
    radii["L5_NBC"] = 9.00336164747;
    radii["L5_SBC"] = 10.0064823627;
    radii["L5_ChC"] = 7.85296694438;
    radii["L6_TPC_L1"] = 7.11837192363;
    radii["L6_TPC_L4"] = 7.08293668837;
    radii["L6_UTPC"] = 7.74583534818;
    radii["L6_IPC"] = 7.71743569988;
    radii["L6_BPC"] = 7.14034402714;
    radii["L6_MC"] = 9.55710192858;
    radii["L6_BTC"] = 9.97289562225;
    radii["L6_DBC"] = 9.88600463867;
    radii["L6_BP"] = 5.98329114914;
    radii["L6_NGC"] = 6.62666320801;
    radii["L6_LBC"] = 9.49501685154;
    radii["L6_NBC"] = 9.33885664259;
    radii["L6_SBC"] = 8.06322860718;
    radii["L6_ChC"] = 8.15173133214;

    const auto& mtypes = circuit.getMorphologyTypeNames();
    std::vector<float> output;
    strings missingMtypes;
    for (auto mtype : mtypes)
    {
        auto iter = radii.find(mtype);
        if (iter == radii.end())
        {
            missingMtypes.push_back(mtype);
            output.push_back(defaultRadius);
        }
        else
            output.push_back(iter->second);
    }

    if (!missingMtypes.empty())
    {
        BRAYNS_WARN << "Soma radius not found for some mtypes:";
        for (const auto& mtype : missingMtypes)
            BRAYNS_WARN << " " << mtype;
        BRAYNS_WARN << std::endl;
    }

    return output;
}

/**
 * Return a vector of gids.size() elements with the layer index of each gid
 */
size_ts _getLayerIds(const brain::Circuit& circuit, const brain::GIDSet& gids)
{
    std::vector<brion::GIDSet> layers;
    try
    {
        for (auto layer : {"1", "2", "3", "4", "5", "6"})
            layers.push_back(circuit.getGIDs(std::string("Layer") + layer));
    }
    catch (const std::runtime_error& e)
    {
        // Some layer was not found by getGIDs.
        BRAYNS_ERROR << "Color scheme neuron_by_layer unavailable: " << e.what()
                     << std::endl;
        return size_ts();
    }

    std::vector<brion::GIDSet::const_iterator> iters;
    for (const auto& layer : layers)
        iters.push_back(layer.begin());

    size_ts materialIds;
    materialIds.reserve(gids.size());
    // For each GID find in which layer GID list it appears, assign
    // this index to its material id list.

    // The following code takes advantage of GID sets being sorted to iterate
    // all of them at once and assign a layer number to each gid in O(n) time.
    for (auto gid : gids)
    {
        size_t i = 0;
        for (i = 0; i != layers.size(); ++i)
        {
            while (iters[i] != layers[i].end() && *iters[i] < gid)
                ++iters[i];
            if (iters[i] != layers[i].end() && *iters[i] == gid)
            {
                materialIds.push_back(i);
                break;
            }
        }
        if (i == layers.size())
            throw std::runtime_error("Layer Id not found for GID " +
                                     std::to_string(gid));
    }
    return materialIds;
}

size_t _getMaterialId(const ColorScheme colorScheme, const uint64_t index,
                      const brain::neuron::SectionType sectionType,
                      const size_ts& perCellMaterialIds)
{
    size_t materialId = 0;
    switch (colorScheme)
    {
    case ColorScheme::neuron_by_id:
        materialId = index;
        break;
    case ColorScheme::neuron_by_segment_type:
        switch (sectionType)
        {
        case brain::neuron::SectionType::soma:
            materialId = 1;
            break;
        case brain::neuron::SectionType::axon:
            materialId = 2;
            break;
        case brain::neuron::SectionType::dendrite:
            materialId = 3;
            break;
        case brain::neuron::SectionType::apicalDendrite:
            materialId = 4;
            break;
        default:
            materialId = 0;
            break;
        }
        break;
    case ColorScheme::neuron_by_target: // no break
    case ColorScheme::neuron_by_etype:  // no break
    case ColorScheme::neuron_by_mtype:  // no break
    case ColorScheme::neuron_by_layer:  // no break
        if (perCellMaterialIds.empty())
        {
            materialId = NO_MATERIAL;
            BRAYNS_DEBUG << "Failed to get per cell material index"
                         << std::endl;
        }
        else
            materialId = perCellMaterialIds[index];
        break;
    default:
        materialId = NO_MATERIAL;
    }
    return materialId;
}

struct CircuitProperties
{
    CircuitProperties() = default;
    CircuitProperties(const PropertyMap& properties)
    {
        const auto setVariable = [&](auto& variable, const std::string& name,
                                     auto defaultVal) {
            using T = typename std::remove_reference<decltype(variable)>::type;
            variable = properties.getProperty<T>(name, defaultVal);
        };

        const auto setEnumVariable =
            [&](auto& variable, const std::string& name, auto defaultVal) {
                using T = decltype(defaultVal);
                const auto enumStr =
                    properties.getProperty<std::string>(name, enumToString<T>(
                                                                  defaultVal));
                variable = stringToEnum<T>(enumStr);
            };

        setVariable(density, PROP_DENSITY.name, 100.0);
        setVariable(randomSeed, PROP_RANDOM_SEED.name, 0);
        setVariable(report, PROP_REPORT.name, "");
        setVariable(targets, PROP_TARGETS.name, "");
        setVariable(meshFilenamePattern, PROP_MESH_FILENAME_PATTERN.name, "");
        setVariable(meshFolder, PROP_MESH_FOLDER.name, "");
        setVariable(useSimulationModel, PROP_USE_SIMULATION_MODEL.name, false);
        setVariable(transformMeshes, PROP_TRANSFORM_MESHES.name, 0);
        setEnumVariable(colorScheme, PROP_COLOR_SCHEME.name, ColorScheme::none);
        setVariable(startSimulationTime, PROP_START_SIMULATION_TIME.name, 0.0);
        setVariable(endSimulationTime, PROP_END_SIMULATION_TIME.name, 0.0);
        setVariable(simulationStep, PROP_SIMULATION_STEP.name, 0.0);
        setVariable(synchronousMode, PROP_SYNCHRONOUS_MODE.name, false);
        setEnumVariable(geometryQuality, PROP_GEOMETRY_QUALITY.name,
                        GeometryQuality::high);

        boost::char_separator<char> separator(",");
        boost::tokenizer<boost::char_separator<char>> tokens(targets,
                                                             separator);
        for_each(tokens.begin(), tokens.end(),
                 [& targetList = targetList](const std::string& s) {
                     targetList.push_back(s);
                 });
    }

    double density = 0.0;
    int32_t randomSeed = 0;
    std::string report;
    std::vector<std::string> targetList;
    std::string targets;
    std::string meshFilenamePattern;
    std::string meshFolder;
    bool useSimulationModel = false;
    bool transformMeshes = false;
    ColorScheme colorScheme = ColorScheme::none;

    double startSimulationTime = 0;
    double endSimulationTime = 0;
    double simulationStep = 0;

    bool synchronousMode = false;

    GeometryQuality geometryQuality = GeometryQuality::high;
};

CompartmentReportPtr _openCompartmentReport(const brain::Simulation* simulation,
                                            const std::string& name,
                                            const brion::GIDSet& gids)
{
    if (!simulation || name.empty())
        return {};

    auto report = simulation->openCompartmentReport(name);
    return std::make_shared<brain::CompartmentReportView>(
        report.createView(gids));
}

void _setSimulationTransferFunction(TransferFunction& tf)
{
    tf.setControlPoints({{0, 1}, {1, 1}});
    // curl https://api.colormaps.io/colormap/unipolar
    tf.setColorMap(
        {"unipolar",
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
          {1.0, 1.0, 1.0}}});
    tf.setValuesRange({-80, -10});
}

SimulationHandlerPtr _createSimulationHandler(
    const CompartmentReportPtr& report, const CircuitProperties& properties)
{
    return std::make_shared<SimulationHandler>(report,
                                               properties.synchronousMode,
                                               properties.startSimulationTime,
                                               properties.endSimulationTime,
                                               properties.simulationStep);
}

class Impl
{
public:
    Impl(Scene& scene, const PropertyMap& properties)
        : _scene(scene)
        , _properties(properties)
        , _morphologyParams(properties)
    {
        _morphologyParams.useSimulationModel = _properties.useSimulationModel;
    }

    ModelDescriptorPtr importCircuit(const std::string& source,
                                     const LoaderProgress& callback) const
    {
        ModelDescriptorPtr modelDesc;
        // Model (one for the whole circuit)
        ModelMetadata metadata = {{"density",
                                   std::to_string(_properties.density)},
                                  {"report", _properties.report},
                                  {"targets", _properties.targets},
                                  {"mesh-filename-pattern",
                                   _properties.meshFilenamePattern},
                                  {"mesh-folder", _properties.meshFolder}};
        auto model = _scene.createModel();

        // Open circuit and simulation if possible
        brion::URI uri(source);
        std::unique_ptr<brain::Simulation> simulation;
        try
        {
            simulation = std::make_unique<brain::Simulation>(uri);
        }
        catch (std::runtime_error& error)
        {
            BRAYNS_INFO << "Cannot open " + source + " as simulation config: "
                        << error.what() << std::endl;
        }
        const auto circuit =
            simulation ? simulation->openCircuit() : brain::Circuit(uri);

        brain::GIDSet allGids;
        size_ts targetSizes;

        const strings localTargets = _properties.targetList.empty()
                                         ? strings{{""}}
                                         : _properties.targetList;

        auto resolveTarget = [&](const std::string& name) {
            const float fraction = _properties.density / 100.0f;
            if (simulation)
                return simulation->getGIDs(name, fraction,
                                           _properties.randomSeed);
            return circuit.getRandomGIDs(fraction, name,
                                         _properties.randomSeed);
        };

        for (const auto& target : localTargets)
        {
            const auto gids = resolveTarget(target);
            if (gids.empty())
            {
                BRAYNS_ERROR << "Target " << target
                             << " does not contain any cells" << std::endl;
                continue;
            }

            BRAYNS_INFO << "Target " << target << ": " << gids.size()
                        << " cells" << std::endl;
            allGids.insert(gids.begin(), gids.end());
            targetSizes.push_back(gids.size());
        }

        if (allGids.empty())
            return {};

        auto compartmentReport =
            _openCompartmentReport(simulation.get(), _properties.report,
                                   allGids);
        const brain::CompartmentReportMapping* reportMapping = nullptr;
        if (compartmentReport)
        {
            model->setSimulationHandler(
                _createSimulationHandler(compartmentReport, _properties));
            // Only keep GIDs from the report
            allGids = compartmentReport->getGIDs();
            reportMapping = &compartmentReport->getMapping();

            _setSimulationTransferFunction(model->getTransferFunction());
        }

        const auto perCellMaterialIds =
            _createPerCellMaterialIds(circuit, allGids, targetSizes);

        if (_morphologyParams.sectionTypes ==
            std::vector<MorphologySectionType>{MorphologySectionType::soma})

        {
            // Import soma only morphologies as spheres
            _addSomaSpheres(circuit, allGids, callback, *model, reportMapping,
                            perCellMaterialIds);
        }
        else
        {
            // Import meshes
            _importMeshes(callback, *model, allGids,
                          circuit.getTransforms(allGids), perCellMaterialIds);

            // Import morphologies
            const auto useSimulationModel = _properties.useSimulationModel;
            model->useSimulationModel(useSimulationModel);
            if (_properties.meshFolder.empty() || useSimulationModel)
            {
                _importMorphologies(circuit, allGids, callback, *model,
                                    reportMapping, perCellMaterialIds);
            }
        }

        // Create materials
        model->createMissingMaterials();
        // Assigning some colors to not get all white
        if (_properties.colorScheme != ColorScheme::none)
        {
            if (_properties.colorScheme == ColorScheme::neuron_by_segment_type)
                // This mode already handles sections types in a hacky way. A
                // refactoring is needed
                model->setMaterialsColorMap(MaterialsColorMap::none);
            else
                model->setMaterialsColorMap(MaterialsColorMap::gradient);
        }

        // Compute circuit center
        auto positions = circuit.getPositions(allGids);
        Boxf center;
        for (const auto& position : positions)
            center.merge(position);

        Transformation transformation;
        transformation.setRotationCenter(center.getCenter());
        modelDesc =
            std::make_shared<ModelDescriptor>(std::move(model), "Circuit",
                                              source, metadata);
        modelDesc->setTransformation(transformation);

        return modelDesc;
    }

private:
    size_ts _createPerCellMaterialIds(const brain::Circuit& circuit,
                                      const brain::GIDSet& gids,
                                      const size_ts& targetSizes) const
    {
        try
        {
            switch (_properties.colorScheme)
            {
            case ColorScheme::neuron_by_target:
            {
                size_ts ids;
                ids.reserve(gids.size());
                size_t id = 0;
                for (const auto size : targetSizes)
                    std::fill_n(std::back_inserter(ids), size, id);
                return ids;
            }
            case ColorScheme::neuron_by_etype:
                return circuit.getElectrophysiologyTypes(gids);
            case ColorScheme::neuron_by_mtype:
                return circuit.getMorphologyTypes(gids);
            case ColorScheme::neuron_by_layer:
                return _getLayerIds(circuit, gids);
            default:
                return size_ts();
            }
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_WARN << "Error creating per cell material ids: " << e.what()
                        << std::endl;
            return size_ts();
        }
    }

    void _addSomaSpheres(const brain::Circuit& circuit,
                         const brain::GIDSet& gids,
                         const LoaderProgress& callback, Model& model,
                         const brain::CompartmentReportMapping* reportMapping,
                         const size_ts& perCellMaterialIds) const
    {
        std::stringstream message;
        message << "Adding " << gids.size() << " spherical somas...";
        const auto positions = circuit.getPositions(gids);
        const auto mtypes = circuit.getMorphologyTypes(gids);

        std::vector<float> mtypeRadii =
            _getSomaRadii(circuit, DEFAULT_SOMA_RADIUS);

        for (size_t i = 0; i != gids.size(); ++i)
        {
            const auto materialId =
                _getMaterialId(_properties.colorScheme, i,
                               brain::neuron::SectionType::soma,
                               perCellMaterialIds);
            const auto offset =
                reportMapping ? reportMapping->getOffsets()[i][0] : 0;
            model.addSphere(materialId,
                            {positions[i], mtypeRadii[mtypes[i]], offset});
            callback.updateProgress(message.str(), i / float(gids.size()));
        }
    }

    void _importMeshes(const LoaderProgress& callback BRAYNS_UNUSED,
                       Model& model BRAYNS_UNUSED,
                       const brain::GIDSet& gids BRAYNS_UNUSED,
                       const Matrix4fs& transformations BRAYNS_UNUSED,
                       const size_ts& perCellMaterialIds BRAYNS_UNUSED) const
    {
#if BRAYNS_USE_ASSIMP
        const auto colorScheme = _properties.colorScheme;
        const auto geometryQuality = _properties.geometryQuality;
        MeshLoader meshLoader(_scene);
        size_t loadingFailures = 0;
        const auto meshedMorphologiesFolder = _properties.meshFolder;
        if (meshedMorphologiesFolder.empty())
            return;

        size_t meshIndex = 0;
        // Loading meshes is currently sequential. TODO: Make it parallel!!!
        std::stringstream message;
        message << "Loading " << gids.size() << " meshes...";
        for (const auto& gid : gids)
        {
            size_t materialId = 0;
            if (colorScheme == ColorScheme::neuron_by_id)
                materialId = meshIndex;
            else if (colorScheme != ColorScheme::neuron_by_segment_type)
            {
                if (perCellMaterialIds.empty())
                    materialId = NO_MATERIAL;
                else
                    materialId = perCellMaterialIds[meshIndex];
            }

            // Load mesh from file
            const auto transformation = _properties.transformMeshes
                                            ? transformations[meshIndex]
                                            : Matrix4f();

            const auto filename =
                _getMeshFilenameFromGID(gid, _properties.meshFilenamePattern,
                                        _properties.meshFolder);
            try
            {
                meshLoader.importMesh(filename, callback, model, meshIndex,
                                      transformation, materialId, colorScheme,
                                      geometryQuality);
            }
            catch (...)
            {
                ++loadingFailures;
            }
            ++meshIndex;
            callback.updateProgress(message.str(),
                                    meshIndex / float(gids.size()));
        }
        if (loadingFailures != 0)
            BRAYNS_WARN << "Failed to import " << loadingFailures << " meshes"
                        << std::endl;
#else
        throw std::runtime_error(
            "assimp dependency is required to load meshes");
#endif
    }

    void _importMorphologies(
        const brain::Circuit& circuit, const brain::GIDSet& gids,
        const LoaderProgress& callback, Model& model,
        const brain::CompartmentReportMapping* reportMapping,
        const size_ts& perCellMaterialIds) const
    {
        MorphologyLoader morphLoader(_scene);

        std::stringstream message;
        message << "Loading " << gids.size() << " morphologies...";

        auto next = gids.begin();
        size_t index = 0;
        std::atomic_size_t current{0};
        std::exception_ptr cancelException;

        while (next != gids.end() && !cancelException)
        {
            brain::GIDSet batch;
            for (; batch.size() < LOAD_BATCH_SIZE && next != gids.end(); ++next)
                batch.insert(*next);

            const auto morphologies =
                circuit.loadMorphologies(batch,
                                         brain::Circuit::Coordinates::global);

#pragma omp parallel for
            for (uint64_t j = 0; j < morphologies.size(); ++j)
            {
                if (cancelException)
                    continue;
                const auto morphologyIndex = index + j;
                auto materialFunc = [&](const brain::neuron::SectionType type) {
                    if (_properties.useSimulationModel)
                        return size_t{0};
                    else
                        return _getMaterialId(_properties.colorScheme,
                                              morphologyIndex, type,
                                              perCellMaterialIds);
                };

                const auto& morphology = morphologies[j];

                auto data =
                    morphLoader.processMorphology(*morphology, morphologyIndex,
                                                  materialFunc, reportMapping,
                                                  _morphologyParams);
#pragma omp critical
                data.addTo(model);

                ++current;
                // Throwing (happens if loading is cancelled) from inside a
                // parallel-for is not allowed.
                try
                {
                    callback.updateProgress(message.str(),
                                            current / float(gids.size()));
                }
                catch (...)
                {
#pragma omp critical
                    cancelException = std::current_exception();
                }
            }
            index += batch.size();
        }

        if (cancelException)
            std::rethrow_exception(cancelException);
    }

private:
    Scene& _scene;
    CircuitProperties _properties;
    MorphologyLoaderParams _morphologyParams;
};
}

namespace brayns
{
CircuitLoader::CircuitLoader(Scene& scene)
    : Loader(scene)
{
}

CircuitLoader::~CircuitLoader()
{
}
bool CircuitLoader::isSupported(const std::string& filename,
                                const std::string& extension
                                    BRAYNS_UNUSED) const
{
    const auto ends_with = [](const std::string& value,
                              const std::string& ending) {
        if (ending.size() > value.size())
            return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    };

    for (const auto& name : LOADER_EXTENSIONS)
        if (ends_with(filename, name))
            return true;

    return false;
}

ModelDescriptorPtr CircuitLoader::importFromBlob(
    Blob&& /*blob*/, const LoaderProgress& /*callback*/,
    const PropertyMap& properties BRAYNS_UNUSED, const size_t /*index*/,
    const size_t /*materialID*/) const
{
    throw std::runtime_error("Loading circuit from blob is not supported");
}

ModelDescriptorPtr CircuitLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap& propertiesTmp, const size_t /*index*/,
    const size_t /*materialID*/) const
{
    callback.updateProgress("Loading circuit ...", 0);

    // Fill property map since the actual property types are known now.
    PropertyMap properties = getProperties();
    properties.merge(propertiesTmp);
    auto impl = Impl(_scene, properties);
    return impl.importCircuit(filename, callback);
}

std::string CircuitLoader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> CircuitLoader::getSupportedExtensions() const
{
    return LOADER_EXTENSIONS;
}

PropertyMap CircuitLoader::getProperties() const
{
    PropertyMap pm;
    pm.setProperty(PROP_DENSITY);
    pm.setProperty(PROP_RANDOM_SEED);
    pm.setProperty(PROP_REPORT);
    pm.setProperty(PROP_TARGETS);
    pm.setProperty(PROP_MESH_FILENAME_PATTERN);
    pm.setProperty(PROP_MESH_FOLDER);
    pm.setProperty(PROP_USE_SIMULATION_MODEL);
    pm.setProperty(PROP_TRANSFORM_MESHES);
    pm.setProperty(PROP_COLOR_SCHEME);
    pm.setProperty(PROP_START_SIMULATION_TIME);
    pm.setProperty(PROP_END_SIMULATION_TIME);
    pm.setProperty(PROP_SIMULATION_STEP);
    pm.setProperty(PROP_SYNCHRONOUS_MODE);
    pm.setProperty(PROP_GEOMETRY_QUALITY);

    { // Add all morphology loader properties
        const auto mlpm = MorphologyLoader(_scene).getProperties();
        for (const auto& prop : mlpm.getProperties())
            if (prop && !pm.hasProperty(prop->name))
                pm.setProperty(*prop);
    }

#if BRAYNS_USE_ASSIMP
    { // Add all mesh loader properties
        const auto mlpm = MeshLoader(_scene).getProperties();
        for (const auto& prop : mlpm.getProperties())
            if (prop && !pm.hasProperty(prop->name))
                pm.setProperty(*prop);
    }
#endif

    return pm;
}
}
