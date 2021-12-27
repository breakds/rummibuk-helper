add_library(tile STATIC)
target_sources(tile
    PUBLIC
    $<INSTALL_INTERFACE:include/rummibuk/solver/tile.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/tile.h>
    PRIVATE tile.cc)
target_link_libraries(tile
    PRIVATE spdlog::spdlog)

cc_test(
    NAME tile_test
    SRCS tile_test.cc
    DEPS tile)
