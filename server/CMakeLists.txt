add_executable(server
    main.cpp
    server_config.h server_config.cpp
    server_core.h server_core.cpp
    server_window.h server_window.cpp

)

target_link_libraries(server
    common_lib
    Qt6::Widgets
    Qt6::Network
)
