add_executable(client
    main.cpp
)

target_link_libraries(client
    common_lib
    Qt6::Widgets
    Qt6::Network
)