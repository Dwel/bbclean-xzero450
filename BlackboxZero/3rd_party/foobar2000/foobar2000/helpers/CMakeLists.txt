add_definitions(-DUNICODE -D_UNICODE)

set(foobar2000helpersSDK_SOURCES
  clipboard.cpp
  create_directory_helper.cpp
  cue_creator.cpp
  cue_parser.cpp
  cue_parser_embedding.cpp
  cuesheet_index_list.cpp
  dialog_resize_helper.cpp
  dropdown_helper.cpp
  dynamic_bitrate_helper.cpp
  file_info_const_impl.cpp
  file_list_helper.cpp
  file_move_helper.cpp
  file_wrapper_simple.cpp
  filetimetools.cpp
  IDataObjectUtils.cpp
  input_helpers.cpp
  listview_helper.cpp
  metadb_io_hintlist.cpp
  mp3_utils.cpp
  seekabilizer.cpp
  StdAfx.cpp
  stream_buffer_helper.cpp
  text_file_loader.cpp
  VisUtils.cpp
  wildcard.cpp
  win32_dialog.cpp
  win32_misc.cpp
  window_placement_helper.cpp
)

add_library(foobar2000helpersSDK STATIC ${foobar2000helpersSDK_SOURCES})
set_property(TARGET foobar2000helpersSDK PROPERTY FOLDER "3rd/foobar2000SDK")
