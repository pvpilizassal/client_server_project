add_executable(client
    main.cpp
    client_config.h client_config.cpp
    client_window.h client_window.cpp
    client_core.h client_core.cpp
    analogclock.h analogclock.cpp
)

target_link_libraries(client
    common_lib
    Qt6::Widgets
    Qt6::Network
)

install(TARGETS client
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET client
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
