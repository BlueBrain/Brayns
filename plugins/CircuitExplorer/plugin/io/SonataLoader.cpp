#include "SonataLoader.h"

#include "AdvancedCircuitLoader.h"

#include <common/types.h>

#include <brion/blueConfig.h>

#include <brayns/common/Log.h>

SonataLoader::SonataLoader(
    brayns::Scene& scene,
    const brayns::ApplicationParameters& applicationParameters,
    brayns::PropertyMap&& loaderParams, CircuitExplorerPlugin* plugin)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams), plugin)
{
    brayns::Log::info("[CE] Registering {}.", getName());
    _fixedDefaults.add(
        {PROP_PRESYNAPTIC_NEURON_GID.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_POSTSYNAPTIC_NEURON_GID.getName(), std::string("")});
    _fixedDefaults.add(PROP_SYNCHRONOUS_MODE);
}

std::string SonataLoader::getName() const
{
    return std::string("Sonata circuit loader");
}

brayns::PropertyMap SonataLoader::getCLIProperties()
{
    brayns::PropertyMap properties;
    properties.add(
        {"populations", std::vector<std::string>(), {"Populations to load"}});
    properties.add(
        {"reports", std::vector<std::string>(), {"Reports to load"}});
    properties.add(
        {"reportTypes", std::vector<std::string>(), {"Report types to load"}});
    auto pm = AdvancedCircuitLoader::getCLIProperties();
    properties.merge(pm);
    return properties;
}

std::vector<brayns::ModelDescriptorPtr> SonataLoader::importFromFile(
    const std::string& filename, const brayns::LoaderProgress& callback,
    const brayns::PropertyMap& properties) const
{
    if (filename.find("BlueConfig") != std::string::npos ||
        filename.find("CircuitConfig") != std::string::npos)
        return _loadFromBlueConfig(filename, callback, properties);

    return std::vector<brayns::ModelDescriptorPtr>();
}

std::vector<brayns::ModelDescriptorPtr> SonataLoader::_loadFromBlueConfig(
    const std::string& file, const brayns::LoaderProgress& cb,
    const brayns::PropertyMap& props) const
{
    std::vector<brayns::ModelDescriptorPtr> result;

    auto& populationNames = props["populations"].as<std::vector<std::string>>();
    auto& populationReports = props["reports"].as<std::vector<std::string>>();
    auto& populationReportTypes =
        props["reportTypes"].as<std::vector<std::string>>();
    const double density = props[PROP_DENSITY.getName()].as<double>();

    if (populationNames.size() != populationReports.size() ||
        populationNames.size() != populationReportTypes.size())
        throw std::runtime_error(
            "Population name count must match report name, report type count");

    for (size_t i = 0; i < populationNames.size(); ++i)
    {
        // Default properties used for each loaded population
        brayns::PropertyMap defaultProperties =
            AdvancedCircuitLoader::getCLIProperties();
        defaultProperties.update(PROP_SECTION_TYPE_APICAL_DENDRITE.getName(),
                                 true);
        defaultProperties.update(PROP_SECTION_TYPE_AXON.getName(), false);
        defaultProperties.update(PROP_SECTION_TYPE_DENDRITE.getName(), true);
        defaultProperties.update(PROP_SECTION_TYPE_SOMA.getName(), true);
        defaultProperties.update(PROP_USER_DATA_TYPE.getName(),
                                 std::string("Simulation offset"));
        defaultProperties.update(PROP_LOAD_LAYERS.getName(), false);
        defaultProperties.update(PROP_LOAD_ETYPES.getName(), false);
        defaultProperties.update(PROP_LOAD_MTYPES.getName(), false);
        defaultProperties.update(PROP_USE_SDF_GEOMETRY.getName(), true);
        defaultProperties.add(PROP_PRESYNAPTIC_NEURON_GID);
        defaultProperties.add(PROP_POSTSYNAPTIC_NEURON_GID);

        // Population variables
        const auto& populationName = populationNames[i];
        const auto& populationReport = populationReports[i];
        const auto& populationReportType = populationReportTypes[i];

        brayns::Log::info("[CE] Loading population {}.", populationName);

        // Use the default parameters and update the variable ones for each
        // population
        defaultProperties.update(PROP_DENSITY.getName(), density);
        defaultProperties.update(PROP_REPORT.getName(), populationReport);
        defaultProperties.update(PROP_REPORT_TYPE.getName(),
                                 populationReportType);

        // Load the BlueConfig/CircuitConfig
        std::unique_ptr<brion::BlueConfig> config;
        // Section Run Default
        if (populationName == "Default")
            config = std::make_unique<brion::BlueConfig>(file);
        // Section Circuit <population name>
        else
            config = std::make_unique<brion::BlueConfig>(
                file, brion::BlueConfigSection::CONFIGSECTION_CIRCUIT,
                populationName);

        // Import the model
        auto model =
            importCircuitFromBlueConfig(*config, defaultProperties, cb);
        if (model)
            result.push_back(model);
    }

    return result;
}
