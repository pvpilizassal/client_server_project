add_executable(client
    main.cpp
    client_config.h client_config.cpp
    client_window.h client_window.cpp
    client_core.h client_core.cpp

)

target_link_libraries(client
    common_lib
    Qt6::Widgets
    Qt6::Network
)
