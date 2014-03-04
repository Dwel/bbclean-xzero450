add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DSTATIC)

set(foobar2000_component_client_SOURCES
  component_client.cpp
)

add_library(foobar2000_component_client STATIC ${foobar2000_component_client_SOURCES})
set_property(TARGET foobar2000_component_client PROPERTY FOLDER "3rd/foobar2000SDK")
