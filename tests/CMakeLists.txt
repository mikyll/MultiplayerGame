# Unit tests: each one links only the game sources it covers

add_executable(test_players test_players.c "${CMAKE_SOURCE_DIR}/src/game/player.c")
target_include_directories(test_players PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(test_players PRIVATE mg_common)
add_test(NAME players COMMAND test_players)
