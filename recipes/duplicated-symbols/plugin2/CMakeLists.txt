add_library(plugin2 SHARED plugin2.cc)

target_include_directories(plugin2 PRIVATE ../common)
target_link_libraries(plugin2 PRIVATE common-v1)
