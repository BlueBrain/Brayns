#include "SonataLoader.h"

#include "AdvancedCircuitLoader.h"

#include <common/log.h>
#include <common/types.h>

#include <brion/blueConfig.h>

SonataLoader::SonataLoader(brayns::Scene &scene,
                           const brayns::ApplicationParameters &applicationParameters,
                           brayns::PropertyMap &&loaderParams,
                           CircuitExplorerPlugin* plugin)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams), plugin)
{
    PLUGIN_INFO << "Registering " << getName() << std::endl;
    _fixedDefaults.setProperty(
        {PROP_PRESYNAPTIC_NEURON_GID.name, std::string("")});
    _fixedDefaults.setProperty(
        {PROP_POSTSYNAPTIC_NEURON_GID.name, std::string("")});
    _fixedDefaults.setProperty(PROP_SYNCHRONOUS_MODE);
}

std::string SonataLoader::getName() const
{
    return std::string("Sonata circuit loader");
}

brayns::PropertyMap SonataLoader::getCLIProperties()
{
    brayns::PropertyMap properties;
    properties.setProperty({"populations", std::vector<std::string>(), {"Populations to load"}});
    properties.setProperty({"reports", std::vector<std::string>(), {"Reports to load"}});
    properties.setProperty({"reportTypes", std::vector<std::string>(), {"Report types to load"}});
    //properties.setProperty({"densities", std::vector<double>(), {"Normalized population densities"}});
    auto pm = AdvancedCircuitLoader::getCLIProperties();
    properties.merge(pm);
    return properties;
}

std::vector<brayns::ModelDescriptorPtr> SonataLoader::importFromFile(
    const std::string& filename, const brayns::LoaderProgress& callback,
    const brayns::PropertyMap& properties) const
{
    if(filename.find("BlueConfig") != std::string::npos
            || filename.find("CircuitConfig") != std::string::npos)
        return _loadFromBlueConfig(filename, callback, properties);

    return std::vector<brayns::ModelDescriptorPtr>();
}

std::vector<brayns::ModelDescriptorPtr>
SonataLoader::_loadFromBlueConfig(const std::string& file, const brayns::LoaderProgress& cb,
                                  const brayns::PropertyMap& props) const
{
    std::vector<brayns::ModelDescriptorPtr> result;

    const std::vector<std::string>& populationNames =
            props.getPropertyRef<std::vector<std::string>>("populations");
    const std::vector<std::string>& populationReports =
            props.getPropertyRef<std::vector<std::string>>("reports");
    const std::vector<std::string>& populationReportTypes =
            props.getPropertyRef<std::vector<std::string>>("reportTypes");
    //const std::vector<double>& populationDensities =
    //        props.getProperty<std::vector<double>>("densities");

    if(populationNames.size() != populationReports.size()
        || populationNames.size() != populationReportTypes.size())
        PLUGIN_THROW("Population name count must match report name, report type count")

    for(size_t i = 0; i < populationNames.size(); ++i)
    {
        // Default properties used for each loaded population
        brayns::PropertyMap defaultProperties = AdvancedCircuitLoader::getCLIProperties();
        defaultProperties.updateProperty(PROP_SECTION_TYPE_APICAL_DENDRITE.name, true);
        defaultProperties.updateProperty(PROP_SECTION_TYPE_AXON.name, true);
        defaultProperties.updateProperty(PROP_SECTION_TYPE_DENDRITE.name, true);
        defaultProperties.updateProperty(PROP_SECTION_TYPE_SOMA.name, true);
        defaultProperties.updateProperty(PROP_USER_DATA_TYPE.name, std::string("Simulation offset"));
        defaultProperties.updateProperty(PROP_LOAD_LAYERS.name, false);
        defaultProperties.updateProperty(PROP_LOAD_ETYPES.name, false);
        defaultProperties.updateProperty(PROP_LOAD_MTYPES.name, false);
        defaultProperties.setProperty(PROP_PRESYNAPTIC_NEURON_GID);
        defaultProperties.setProperty(PROP_POSTSYNAPTIC_NEURON_GID);

        // Population variables
        const auto& populationName = populationNames[i];
        const auto& populationReport = populationReports[i];
        const auto& populationReportType = populationReportTypes[i];
        const auto density = 1.0;

        PLUGIN_INFO << "Loading population " << populationName << std::endl;

        // Use the default parameters and update the variable ones for each population
        defaultProperties.updateProperty(PROP_DENSITY.name, density);
        defaultProperties.updateProperty(PROP_REPORT.name, populationReport);
        defaultProperties.updateProperty(PROP_REPORT_TYPE.name, populationReportType);

        // Load the BlueConfig/CircuitConfig
        std::unique_ptr<brion::BlueConfig> config;
        // Section Run Default
        if(populationName == "Default")
            config = std::make_unique<brion::BlueConfig>(file);
        // Section Circuit <population name>
        else
            config = std::make_unique<brion::BlueConfig>(
                        file, brion::BlueConfigSection::CONFIGSECTION_CIRCUIT, populationName);

        std::cout << 1 << std::endl;

        // Import the model
        auto model = importCircuitFromBlueConfig(*config, defaultProperties, cb);
        if(model)
            result.push_back(model);

    }

    return result;
}
