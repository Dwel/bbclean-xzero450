set(WOW64Adapter_SOURCES
  Wow64Adapter.cpp
)

set(WOW64Adapter_HEADERS
	Wow64Adapter.h
)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
add_library(WOW64Adapter SHARED 
            ${WOW64Adapter_SOURCES}
			${WOW64Adapter_HEADERS})

target_link_libraries(WOW64Adapter blackbox bblib)
target_link_libraries(WOW64Adapter version comctl32 gdiplus winmm msimg32 shlwapi comdlg32)

set_property(TARGET WOW64Adapter PROPERTY FOLDER "pluginloaders/WOW64Adapter")

install(TARGETS WOW64Adapter RUNTIME DESTINATION pluginloaders/WOW64Adapter)
