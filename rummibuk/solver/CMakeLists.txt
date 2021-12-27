add_library(tile STATIC)
target_sources(tile
    PUBLIC
    $<INSTALL_INTERFACE:include/rummibuk/solver/tile.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/tile.h>
    PRIVATE tile.cc)
target_link_libraries(tile
    PRIVATE spdlog::spdlog absl::strings)

cc_test(
    NAME tile_test
    SRCS tile_test.cc
    DEPS tile)

add_library(search STATIC)
target_sources(search
    PUBLIC
    $<INSTALL_INTERFACE:include/rummibuk/solver/search.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/search.h>
    PRIVATE search.cc)
target_link_libraries(search
    PRIVATE spdlog::spdlog tile)

cc_test(
    NAME search_test
    SRCS search_test.cc
    DEPS search)
