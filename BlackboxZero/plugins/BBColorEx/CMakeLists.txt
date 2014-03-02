set(bbColorEx_SOURCES
  BBColorEx.cpp
)

#set(bbColorEx_CONFIGS bbColorEx.rc)

#set(bbColorEx_RESOURCES resource.rc)
#set(CMAKE_RC_COMPILER_INIT windres)
#ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbColorEx SHARED ${bbColorEx_SOURCES} ${bbColorEx_RESOURCES})
target_link_libraries(bbColorEx blackbox bblib bbPlugin)
target_link_libraries(bbColorEx version comctl32)

set_property(TARGET bbColorEx PROPERTY FOLDER "plugins/bbColorEx")

#install(FILES readme.txt DESTINATION plugins/bbColorEx)
install(TARGETS bbColorEx RUNTIME DESTINATION plugins/bbColorEx)
#if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbColorEx/${bbColorEx_CONFIGS}")
#	install(FILES ${bbColorEx_CONFIGS} DESTINATION plugins/bbColorEx)
#endif()

