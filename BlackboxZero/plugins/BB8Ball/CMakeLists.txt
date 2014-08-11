include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

set(BB8Ball_SOURCES
  BB8Ball.cpp
)

SET(BB8Ball_HEADERS
	BB8Ball.h
	Resource.h
)

#set(BB8Ball_RESOURCES 
#	Resource.rc
#)

#readme.txt
#Mortar.ico
#BB8Ball.html

#add_definitions(-DUNICODE)
add_library(BB8Ball SHARED ${BB8Ball_SOURCES} ${BB8Ball_HEADERS} ${BB8Ball_RESOURCES})
target_link_libraries(BB8Ball blackbox bblib bbPlugin)
target_link_libraries(BB8Ball version comctl32 gdiplus winmm msimg32 shlwapi comdlg32)

set_property(TARGET BB8Ball PROPERTY FOLDER "plugins/bb8ball")

#install(FILES changes.txt DESTINATION plugins/BB8Ball)
install(TARGETS BB8Ball RUNTIME DESTINATION plugins/BB8Ball)

