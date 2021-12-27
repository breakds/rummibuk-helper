add_executable(crok)
target_sources(crok PRIVATE crok.cc)
target_link_libraries(crok PRIVATE spdlog::spdlog fmt::fmt)
install(TARGETS crok DESTINATION bin)
