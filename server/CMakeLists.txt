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

install(TARGETS server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET server
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
