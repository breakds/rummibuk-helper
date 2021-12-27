add_executable(brok)
target_sources(brok PRIVATE brok.cc)
target_link_libraries(brok PRIVATE spdlog::spdlog fmt::fmt)
install(TARGETS brok DESTINATION bin)
