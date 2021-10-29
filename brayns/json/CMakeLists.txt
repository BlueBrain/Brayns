set(BRAYNSJSON_HEADERS
  Json.h
  JsonAdapter.h
  JsonBuffer.h
  JsonExtractor.h
  JsonOptions.h
  JsonSchema.h
  JsonSchemaValidator.h
  JsonType.h
  Message.h
  MessageAdapter.h
)

set(BRAYNSJSON_SOURCES
    JsonSchemaValidator.cpp
)

set(BRAYNSJSON_INCLUDE_NAME brayns/json)
set(BRAYNSJSON_LINK_LIBRARIES
  PUBLIC Poco::Foundation Poco::JSON glm
  PRIVATE braynsUtils
)

common_library(braynsJson)
