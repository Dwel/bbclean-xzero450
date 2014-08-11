function(install_if_absent)
	foreach(f ${ARGN})
		if(NOT EXISTS \"${CMAKE_INSTALL_PREFIX}/${f}\")
			install(FILES ${f} DESTINATION .)
		endif()
	endforeach()
endfunction()

