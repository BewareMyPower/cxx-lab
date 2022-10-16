add_library(plugin1 SHARED plugin1.cc)

target_include_directories(plugin1 PRIVATE ../common)
target_link_libraries(plugin1 PRIVATE common)
