add_executable(example example.cc)

target_include_directories(example PRIVATE ../plugin1)
target_include_directories(example PRIVATE ../plugin2)
target_link_libraries(example PRIVATE plugin1)
target_link_libraries(example PRIVATE plugin2)
