#pragma once

#include <plugin/api/CircuitColorData.h>

class VasculatureColorData : public CircuitColorData
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
     * @brief Returns the name of the method to color vasculature by section
     * @return std::string
     */
    std::string vasculatureSectionMethodName() const noexcept;

    /**
     * @brief getVasculatureMethods must return the methods which are available by the vasculature data on disk
     * @return std::vector<std::string>
     */
    virtual std::vector<std::string> getVasculatureMethods() const noexcept;

    /**
     * @brief getVasculatureMethodVariables must return the available variables of the given method
     * @param method
     * @return std::vector<std::string>
     */
    virtual std::vector<std::string> getVasculatureMethodVariables(const std::string &method) const;
};
