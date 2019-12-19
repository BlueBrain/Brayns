#include "CircuitInfoParams.h"
#include "Log.h"

#include "json.hpp"

#ifndef BRAYNS_DEBUG_JSON_ENABLED
#define FROM_JSON(PARAM, JSON, NAME) \
    PARAM.NAME = JSON[#NAME].get<decltype(PARAM.NAME)>()
#else
#define FROM_JSON(PARAM, JSON, NAME) \
    try { \
        PARAM.NAME = JSON[#NAME].get<decltype(PARAM.NAME)>(); \
    } \
    catch(...){ \
        PLUGIN_ERROR << "JSON parsing error for attribute '" << #NAME<< "'!" << std::endl; \
        throw; \
    }
#endif
#define TO_JSON(PARAM, JSON, NAME) JSON[#NAME] = PARAM.NAME

bool from_json(TestRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, message);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const TestResponse& response)
{
    try
    {
        nlohmann::json js;

        TO_JSON(response, js, answer);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(CellGIDListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
        FROM_JSON(request, js, targets);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool from_json(ModelCellGIDListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, modelId);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const CellGIDList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, ids);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(ReportListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const ReportList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, reports);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(TargetListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const TargetList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, targets);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(AfferentGIDListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
        FROM_JSON(request, js, sources);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const AfferentGIDList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, ids);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(EfferentGIDListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
        FROM_JSON(request, js, sources);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const EfferentGIDList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, ids);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(ProjectionListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const ProjectionList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, projections);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(ProjectionAfferentGIDListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
        FROM_JSON(request, js, projection);
        FROM_JSON(request, js, sources);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const ProjectionAfferentGIDList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, ids);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(ProjectionEfferentGIDListRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, path);
        FROM_JSON(request, js, projection);
        FROM_JSON(request, js, sources);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string to_json(const ProjectionEfferentGIDList& list)
{
    try
    {
        nlohmann::json js;

        TO_JSON(list, js, ids);
        TO_JSON(list, js, error);
        TO_JSON(list, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}
