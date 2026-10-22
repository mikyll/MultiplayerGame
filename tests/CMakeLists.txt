# Unit tests: each one links only the game sources it covers

add_executable(test_players test_players.c "${CMAKE_SOURCE_DIR}/src/game/player.c")
target_include_directories(test_players PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(test_players PRIVATE mg_common)
add_test(NAME players COMMAND test_players)

add_executable(test_protocol test_protocol.c "${CMAKE_SOURCE_DIR}/src/networking/protocol.c")
target_include_directories(test_protocol PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(test_protocol PRIVATE mg_common)
add_test(NAME protocol COMMAND test_protocol)

# End-to-end network test: headless server, clients and raw ENet probes on localhost (Unix only)
if (UNIX)
    set(MG_NET_SOURCES
        "${CMAKE_SOURCE_DIR}/src/networking/net.c"
        "${CMAKE_SOURCE_DIR}/src/networking/server.c"
        "${CMAKE_SOURCE_DIR}/src/networking/client.c"
        "${CMAKE_SOURCE_DIR}/src/networking/protocol.c"
        "${CMAKE_SOURCE_DIR}/src/game/netsync.c"
        "${CMAKE_SOURCE_DIR}/src/game/player.c"
        "${CMAKE_SOURCE_DIR}/src/utils/utils.c"
    )

    add_executable(net_server net/net_server.c ${MG_NET_SOURCES})
    add_executable(net_client net/net_client.c ${MG_NET_SOURCES})
    add_executable(net_probe net/net_probe.c "${CMAKE_SOURCE_DIR}/src/networking/protocol.c")
    foreach (target net_server net_client net_probe)
        target_include_directories(${target} PRIVATE "${CMAKE_SOURCE_DIR}/src")
        target_link_libraries(${target} PRIVATE mg_common)
    endforeach()

    add_test(NAME network COMMAND bash "${CMAKE_CURRENT_SOURCE_DIR}/net/run.sh" "$<TARGET_FILE_DIR:net_server>")
    set_tests_properties(network PROPERTIES TIMEOUT 60)
endif()
