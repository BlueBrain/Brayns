#pragma once

#include <plugin/api/CircuitColorData.h>

class NeuronColorData : public CircuitColorData
{
public:
    /**
     * @brief getMethods
     * @return
     */
    std::vector<std::string> getMethods() const noexcept final;

    /**
     * @brief getMethodVariables
     * @param method
     * @return
     */
    std::vector<std::string> getMethodVariables(const std::string &method) const final;

    /**
     * @brief Returns the name of the method to color neurons and astorcytes by section
     * @return std::string
     */
    std::string morphologySectionMethodName() const noexcept;

    /**
     * @brief getCircuitMethods must return the methods which are available by the circuit data on disk
     * @return std::vector<std::string>
     */
    virtual std::vector<std::string> getCircuitMethods() const noexcept = 0;

    /**
     * @brief getCircuitMethodVariables must return the available variables of the given method
     * @param method
     * @return std::vector<std::string>
     */
    virtual std::vector<std::string> getCircuitMethodVariables(const std::string &method) const = 0;
};
