# bbLeanSkinEng32.dll and bbLeanSkinRun32.exe on 64b system
cmake_minimum_required(VERSION 2.8)

add_executable(bbLeanSkinRun32
  WIN32
  ../bbLeanSkinRun32.cpp
)

add_library(bbLeanSkinEng32
  SHARED
  ../engine/hookctl.cpp
  ../engine/subclass.cpp
  ../../bbPlugin/drawico.cpp # FIXME: required DrawIconSatnHue
)

#FIXME: the paths are bit messed, this is a hotfix
include_directories(${CMAKE_SOURCE_DIR}/../../../blackbox)
include_directories(${CMAKE_SOURCE_DIR}/../../../lib)
include_directories(${CMAKE_SOURCE_DIR}/../../../plugins/bbPlugin)

target_link_libraries(bbLeanSkinEng32 version comctl32)
target_link_libraries(bbLeanSkinRun32 bbLeanSkinEng32 version comctl32)

install(TARGETS bbLeanSkinEng32 RUNTIME DESTINATION plugins/bbLeanSkin)
install(TARGETS bbLeanSkinRun32 RUNTIME DESTINATION plugins/bbLeanSkin)

###############################################################################
# debug stuff
#message("Debug variable dump:")
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#   message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
