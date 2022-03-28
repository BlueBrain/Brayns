#include "VasculatureColorData.h"

#include <plugin/io/morphology/vasculature/VasculatureSection.h>

std::vector<std::string> VasculatureColorData::getMethods() const noexcept
{
    auto result = getVasculatureMethods();
    result.push_back(vasculatureSectionMethodName());
    return result;
}

std::vector<std::string> VasculatureColorData::getMethodVariables(const std::string &method) const
{
    if(method == vasculatureSectionMethodName())
    {
        return brayns::enumNames<VasculatureSection>();
    }
    else
    {
        return getVasculatureMethodVariables(method);
    }
}

std::string VasculatureColorData::vasculatureSectionMethodName() const noexcept
{
    return "vasculature section";
}

std::vector<std::string> VasculatureColorData::getVasculatureMethods() const noexcept
{
    return {};
}

std::vector<std::string> VasculatureColorData::getVasculatureMethodVariables(const std::string &method) const
{
    (void) method;
    return {};
}
