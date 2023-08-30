cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_C_STANDARD 99)
set(DEBUG 0 CACHE STRING "Debug level")
set(USE_SAM 0 CACHE STRING "Use SAM for card reading")
set(USE_NFC 1 CACHE STRING "Use NFC for card reading")

configure_file(config.h.in config.h)

project(id-chip-reader LANGUAGES C)

include(GNUInstallDirs)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")

add_library(id_chip_reader STATIC ${SOURCES})

target_include_directories(id_chip_reader PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS id_chip_reader
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(DIRECTORY include/
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

option(ID_CHIP_READER_EXAMPLES "Build examples" TRUE)

if(ID_CHIP_READER_EXAMPLES)
  add_subdirectory(example)
endif()