if(USE_LOGGING EQUAL 1)
  add_subdirectory(DbgToolkit)
  add_subdirectory(DbgToolkit/trace_client)
endif(USE_LOGGING EQUAL 1)
add_subdirectory(logging)
add_subdirectory(tcl8.6.4/win)

if (NOT MINGW)
	add_subdirectory(foobar2000)
	add_subdirectory(iTunesCOMWindowsSDK)
endif (NOT MINGW)


