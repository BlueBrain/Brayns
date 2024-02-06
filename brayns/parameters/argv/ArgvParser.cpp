/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "ArgvParser.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <brayns/utils/string/StringSplitter.h>

namespace
{
using ArgvMap = std::unordered_map<std::string, std::vector<brayns::ArgvValue>>;

class ArgvStream
{
public:
    ArgvStream(int argc, const char **argv):
        _argc(argc),
        _argv(argv)
    {
    }

    bool hasCurrent() const
    {
        return _index < _argc;
    }

    bool hasNext() const
    {
        return _index < _argc - 1;
    }

    const char *getCurrent() const
    {
        assert(hasCurrent());
        return _argv[_index];
    }

    void next()
    {
        ++_index;
    }

private:
    int _index = 1;
    int _argc = 0;
    const char **_argv = nullptr;
};

class ArgvStreamExtractor
{
public:
    static std::string extractKey(ArgvStream &stream)
    {
        auto token = stream.getCurrent();
        if (!_isKey(token))
        {
            throw std::runtime_error("Unknown positional argument '" + std::string(token) + "'");
        }
        auto key = _extractKey(token);
        if (!stream.hasNext())
        {
            throw std::runtime_error("No value for keyword argument '" + key + "'");
        }
        stream.next();
        return key;
    }

    static std::string extractValue(ArgvStream &stream)
    {
        auto token = stream.getCurrent();
        stream.next();
        return token;
    }

private:
    static bool _isKey(const char *token)
    {
        return token[0] == '-' && token[1] == '-';
    }

    static std::string _extractKey(const char *value)
    {
        return value + 2;
    }
};

class ArgvItemCountValidator
{
public:
    explicit ArgvItemCountValidator(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    void validate(const std::vector<brayns::ArgvValue> &values)
    {
        auto itemCount = values.size();
        auto minItems = _property.minItems;
        if (minItems && itemCount < *minItems)
        {
            throw std::runtime_error(_notEnoughItems(itemCount, *minItems));
        }
        auto maxItems = _property.maxItems;
        if (maxItems && itemCount > *maxItems)
        {
            throw std::runtime_error(_tooManyItems(itemCount, *maxItems));
        }
    }

private:
    const brayns::ArgvProperty &_property;

    std::string _notEnoughItems(size_t itemCount, size_t minItems)
    {
        std::ostringstream stream;
        stream << "Not enough items for keyword argument '" << _property.name;
        stream << "': " << itemCount << " < " << minItems;
        return stream.str();
    }

    std::string _tooManyItems(size_t itemCount, size_t maxItems)
    {
        std::ostringstream stream;
        stream << "Too many items for keyword argument '" << _property.name;
        stream << "': " << itemCount << " > " << maxItems;
        return stream.str();
    }
};

class ArgvNumberExtractor
{
public:
    explicit ArgvNumberExtractor(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    double extract(const std::string &token)
    {
        double value;
        std::istringstream stream(token);
        stream >> value;
        if (stream.fail())
        {
            throw std::runtime_error(_invalidNumber(token));
        }
        _validate(value);
        return value;
    }

private:
    const brayns::ArgvProperty &_property;

    std::string _invalidNumber(const std::string &token)
    {
        std::ostringstream stream;
        stream << "Invalid numeric value for keyword argument '" << _property.name;
        stream << "': '" << token << "'";
        return stream.str();
    }

    void _validate(double value)
    {
        auto minimum = _property.minimum;
        if (minimum && value < *minimum)
        {
            throw std::runtime_error(_belowMinimum(value, *minimum));
        }
        auto maximum = _property.maximum;
        if (maximum && value > *maximum)
        {
            throw std::runtime_error(_aboveMaximum(value, *maximum));
        }
    }

    std::string _belowMinimum(double value, double minimum)
    {
        std::ostringstream stream;
        stream << "Value below minimum for keyword argument '" << _property.name;
        stream << "': " << value << " < " << minimum;
        return stream.str();
    }

    std::string _aboveMaximum(double value, double maximum)
    {
        std::ostringstream stream;
        stream << "Value above maximum for keyword argument '" << _property.name;
        stream << "': " << value << " > " << maximum;
        return stream.str();
    }
};

class ArgvEnumParser
{
public:
    explicit ArgvEnumParser(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    bool canParse() const
    {
        return !_property.enums.empty();
    }

    brayns::ArgvValue parse(const std::string &token)
    {
        _validate(token);
        return brayns::ArgvValue(token);
    }

private:
    const brayns::ArgvProperty &_property;

    void _validate(const std::string &token)
    {
        auto &enums = _property.enums;
        auto i = std::find(enums.begin(), enums.end(), token);
        if (i != enums.end())
        {
            return;
        }
        throw std::runtime_error(_invalidEnum(token));
    }

    std::string _invalidEnum(const std::string &token)
    {
        std::ostringstream stream;
        stream << "Invalid enum value for keyword argument '" << _property.name;
        stream << "': '" << token << "' not in [";
        bool first = true;
        for (const auto &value : _property.enums)
        {
            if (!first)
            {
                stream << ", ";
            }
            first = false;
            stream << value;
        }
        stream << "]";
        return stream.str();
    }
};

class ArgvBooleanParser
{
public:
    explicit ArgvBooleanParser(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    bool canParse() const
    {
        return _property.type == brayns::ArgvType::Boolean;
    }

    brayns::ArgvValue parse(const std::string &token)
    {
        if (token == "true" || token == "1")
        {
            return brayns::ArgvValue(true);
        }
        if (token == "false" || token == "0")
        {
            return brayns::ArgvValue(false);
        }
        throw std::runtime_error(_invalidBoolean(token));
    }

private:
    const brayns::ArgvProperty &_property;

    std::string _invalidBoolean(const std::string &token)
    {
        std::ostringstream stream;
        stream << "Invalid boolean value for keyword argument '" << _property.name;
        stream << "': '" << token << "'";
        return stream.str();
    }
};

class ArgvIntegerParser
{
public:
    explicit ArgvIntegerParser(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    bool canParse() const
    {
        return _property.type == brayns::ArgvType::Integer;
    }

    brayns::ArgvValue parse(const std::string &token)
    {
        auto extractor = ArgvNumberExtractor(_property);
        auto value = extractor.extract(token);
        _checkIsInteger(value);
        return brayns::ArgvValue(static_cast<int64_t>(value));
    }

private:
    const brayns::ArgvProperty &_property;

    void _checkIsInteger(double value)
    {
        if (std::floor(value) != value)
        {
            throw std::runtime_error(_invalidInteger(value));
        }
    }

    std::string _invalidInteger(double value)
    {
        std::ostringstream stream;
        stream << "Invalid integer value for keyword argument '" << _property.name;
        stream << "': " << value;
        return stream.str();
    }
};

class ArgvNumberParser
{
public:
    explicit ArgvNumberParser(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    bool canParse() const
    {
        return _property.type == brayns::ArgvType::Number;
    }

    brayns::ArgvValue parse(const std::string &token)
    {
        auto extractor = ArgvNumberExtractor(_property);
        auto number = extractor.extract(token);
        return brayns::ArgvValue(number);
    }

private:
    const brayns::ArgvProperty &_property;
};

class ArgvStringParser
{
public:
    explicit ArgvStringParser(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    bool canParse() const
    {
        return _property.type == brayns::ArgvType::String;
    }

    brayns::ArgvValue parse(const std::string &token)
    {
        return brayns::ArgvValue(token);
    }

private:
    const brayns::ArgvProperty &_property;
};

class ArgvValueParser
{
public:
    explicit ArgvValueParser(const brayns::ArgvProperty &property):
        _property(property)
    {
    }

    void parse(const std::string &token, std::vector<brayns::ArgvValue> &values)
    {
        _checkComposition(values);
        auto tokens = _split(token);
        if (!_parse(tokens, values))
        {
            throw std::runtime_error("Internal error: invalid property");
        }
    }

private:
    const brayns::ArgvProperty &_property;

    void _checkComposition(std::vector<brayns::ArgvValue> &values)
    {
        if (!values.empty() && !_property.composable)
        {
            throw std::runtime_error("Duplicated keyword argument '" + _property.name + "'");
        }
    }

    std::vector<std::string> _split(const std::string &token)
    {
        if (!_property.multitoken)
        {
            return {token};
        }
        return brayns::StringSplitter::split(token, ' ');
    }

    bool _parse(const std::vector<std::string> &tokens, std::vector<brayns::ArgvValue> &values)
    {
        return _tryParse<ArgvEnumParser>(tokens, values) || _tryParse<ArgvBooleanParser>(tokens, values)
            || _tryParse<ArgvIntegerParser>(tokens, values) || _tryParse<ArgvNumberParser>(tokens, values)
            || _tryParse<ArgvStringParser>(tokens, values);
    }

    template<typename T>
    bool _tryParse(const std::vector<std::string> &tokens, std::vector<brayns::ArgvValue> &values)
    {
        auto parser = T(_property);
        if (!parser.canParse())
        {
            return false;
        }
        for (const auto &token : tokens)
        {
            auto value = parser.parse(token);
            values.push_back(value);
        }
        ArgvItemCountValidator validator(_property);
        validator.validate(values);
        return true;
    }
};

class ArgvParserHelper
{
public:
    explicit ArgvParserHelper(const std::vector<brayns::ArgvProperty> &properties):
        _properties(properties)
    {
    }

    ArgvMap parse(ArgvStream &stream)
    {
        ArgvMap argv;
        while (stream.hasCurrent())
        {
            _parseArgument(stream, argv);
        }
        return argv;
    }

private:
    const std::vector<brayns::ArgvProperty> &_properties;

    void _parseArgument(ArgvStream &stream, ArgvMap &argv)
    {
        auto key = ArgvStreamExtractor::extractKey(stream);
        auto &property = _getProperty(key);
        auto value = ArgvStreamExtractor::extractValue(stream);
        auto parser = ArgvValueParser(property);
        parser.parse(value, argv[key]);
    }

    const brayns::ArgvProperty &_getProperty(const std::string &key)
    {
        auto first = _properties.begin();
        auto last = _properties.end();
        auto predictor = [&](auto &property) { return property.name == key; };
        auto i = std::find_if(first, last, predictor);
        if (i == last)
        {
            throw std::runtime_error("Unknown keyword argument '" + key + "'");
        }
        return *i;
    }
};

class ArgvLoader
{
public:
    explicit ArgvLoader(const std::vector<brayns::ArgvProperty> &properties):
        _properties(properties)
    {
    }

    void load(const ArgvMap &argv)
    {
        for (const auto &property : _properties)
        {
            auto i = argv.find(property.name);
            if (i == argv.end())
            {
                continue;
            }
            property.load(i->second);
        }
    }

private:
    const std::vector<brayns::ArgvProperty> &_properties;
};
} // namespace

namespace brayns
{
ArgvParser::ArgvParser(const std::vector<ArgvProperty> &properties):
    _properties(properties)
{
}

void ArgvParser::parse(int argc, const char **argv)
{
    ArgvParserHelper parser(_properties);
    ArgvStream stream(argc, argv);
    auto result = parser.parse(stream);
    ArgvLoader loader(_properties);
    loader.load(result);
}
} // namespace brayns
