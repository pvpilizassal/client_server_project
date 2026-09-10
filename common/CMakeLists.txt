add_library(common_lib STATIC
    config_base.h config_base.cpp
    protocol.h
)

target_link_libraries(common_lib Qt6::Core)

target_include_directories(common_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
