add_executable(server
    main.cpp
)

target_link_libraries(server
    common_lib
    Qt6::Widgets
    Qt6::Network
)