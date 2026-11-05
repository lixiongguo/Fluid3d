add_executable(hybrid_liquid hybrid_liquid/main.cpp)
target_link_libraries(hybrid_liquid jet)

add_executable(smoke smoke/main.cpp)
target_link_libraries(smoke jet)

add_executable(sph_drop sph_drop/main.cpp)
target_link_libraries(sph_drop jet)
