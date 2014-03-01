add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DSTATIC)

set(foobar2000pfcSDK_SOURCES
  base64.cpp
  bsearch.cpp
  guid.cpp
  other.cpp
  pathUtils.cpp
  printf.cpp
  profiler.cpp
  selftest.cpp
  sort.cpp
  stdafx.cpp
  string.cpp
  string_conv.cpp
  stringNew.cpp
  threads.cpp
  utf8.cpp
)

add_library(foobar2000pfcSDK STATIC ${foobar2000pfcSDK_SOURCES})

set_property(TARGET foobar2000pfcSDK PROPERTY FOLDER "3rd/foobar2000SDK")
