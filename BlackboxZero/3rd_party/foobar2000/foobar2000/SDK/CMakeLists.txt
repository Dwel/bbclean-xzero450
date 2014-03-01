
add_definitions(-DUNICODE -D_UNICODE)

set(foobar2000SDK_SOURCES
  abort_callback.cpp
  advconfig.cpp
  album_art.cpp
  app_close_blocker.cpp
  audio_chunk.cpp
  audio_chunk_channel_config.cpp
  cfg_var.cpp
  chapterizer.cpp
  commandline.cpp
  completion_notify.cpp
  config_object.cpp
  console.cpp
  dsp.cpp
  dsp_manager.cpp
  file_info.cpp
  file_info_impl.cpp
  file_info_merge.cpp
  file_operation_callback.cpp
  filesystem.cpp
  filesystem_helper.cpp
  guids.cpp
  hasher_md5.cpp
  input.cpp
  input_file_type.cpp
  link_resolver.cpp
  mainmenu.cpp
  mem_block_container.cpp
  menu_helpers.cpp
  menu_item.cpp
  menu_manager.cpp
  metadb.cpp
  metadb_handle.cpp
  metadb_handle_list.cpp
  packet_decoder.cpp
  playable_location.cpp
  playback_control.cpp
  playlist.cpp
  playlist_loader.cpp
  popup_message.cpp
  preferences_page.cpp
  replaygain.cpp
  replaygain_info.cpp
  service.cpp
  stdafx.cpp
  tag_processor.cpp
  tag_processor_id3v2.cpp
  threaded_process.cpp
  titleformat.cpp
  ui.cpp
  ui_element.cpp
)

add_library(foobar2000SDK STATIC ${foobar2000SDK_SOURCES})

set_property(TARGET foobar2000SDK PROPERTY FOLDER "3rd/foobar2000SDK")
