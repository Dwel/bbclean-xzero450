# bbLeanSkin.dll

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbLeanSkin SHARED bbLeanSkin.cpp)
set_target_properties(bbLeanSkin PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
set_target_properties(bbLeanSkin PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_target_properties(bbLeanSkin PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
set_target_properties(bbLeanSkin PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
target_link_libraries(bbLeanSkin blackbox bblib bbPlugin)
target_link_libraries(bbLeanSkin version comctl32)
set_property(TARGET bbLeanSkin PROPERTY FOLDER "plugins/bbLeanSkin")
install(FILES readme.txt DESTINATION plugins/bbLeanSkin)
install(TARGETS bbLeanSkin RUNTIME DESTINATION plugins/bbLeanSkin)
install_if_absent(plugins/bbLeanSkin bbLeanSkin.rc bbLeanSkinMenu.rc exclusions.rc)
installdir_if_absent(plugins/bbLeanSkin buttons)

# bbLeanSkinEng.dll

set(target_name "bbLeanSkinEng")
add_library(${target_name}
	SHARED
	engine/hookctl.cpp
	engine/subclass.cpp
)
set_target_properties(bbLeanSkinEng PROPERTIES LINK_FLAGS_DEBUG          "/SUBSYSTEM:WINDOWS")
set_target_properties(bbLeanSkinEng PROPERTIES LINK_FLAGS_RELEASE        "/SUBSYSTEM:WINDOWS")
set_target_properties(bbLeanSkinEng PROPERTIES LINK_FLAGS_MINSIZEREL     "/SUBSYSTEM:WINDOWS")
set_target_properties(bbLeanSkinEng PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
target_link_libraries(${target_name} blackbox bblib bbPlugin)
target_link_libraries(${target_name} version comctl32)
install(TARGETS ${target_name} RUNTIME DESTINATION plugins/bbLeanSkin)
set_property(TARGET ${target_name} PROPERTY FOLDER "plugins/bbLeanSkin")

