cmake_minimum_required(VERSION 3.16)

project(MultiplayerGame VERSION 0.6.0 LANGUAGES C)

option(MG_WERROR "Treat compiler warnings as errors" OFF)

file(GLOB_RECURSE MG_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE MG_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h")

add_executable(MultiplayerGame ${MG_SOURCES} ${MG_HEADERS})
set_target_properties(MultiplayerGame PROPERTIES
    OUTPUT_NAME multiplayer_game
    C_STANDARD 11
)

# Mirror src/ folders in the IDE (e.g. Visual Studio Solution Explorer)
source_group(TREE "${CMAKE_SOURCE_DIR}/src" FILES ${MG_SOURCES} ${MG_HEADERS})

# Dependencies: SDL2, SDL2_image, ENet
if (WIN32)
    # Headers and import libraries are shipped in libs/ (DLLs must sit next to the executable)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MG_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/lib/x64")
        set(MG_ENET_LIB enet64)
    else()
        set(MG_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/lib/x86")
        set(MG_ENET_LIB enet)
    endif()

    target_include_directories(MultiplayerGame PRIVATE "${CMAKE_SOURCE_DIR}/libs/include")
    target_link_libraries(MultiplayerGame PRIVATE
        "${MG_LIB_DIR}/SDL2main.lib"
        "${MG_LIB_DIR}/SDL2.lib"
        "${MG_LIB_DIR}/SDL2_image.lib"
        "${MG_LIB_DIR}/${MG_ENET_LIB}.lib"
        winmm
        ws2_32
    )
    target_compile_definitions(MultiplayerGame PRIVATE _CONSOLE _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    find_package(SDL2 REQUIRED CONFIG)
    find_package(SDL2_image REQUIRED CONFIG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ENET REQUIRED IMPORTED_TARGET libenet)

    target_link_libraries(MultiplayerGame PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        PkgConfig::ENET
        m
    )
endif()

# Warnings
if (MSVC)
    target_compile_options(MultiplayerGame PRIVATE /W3 $<$<BOOL:${MG_WERROR}>:/WX>)
else()
    target_compile_options(MultiplayerGame PRIVATE
        -Wall -Wempty-body -Wstrict-prototypes -Werror=maybe-uninitialized -Warray-bounds
        $<$<BOOL:${MG_WERROR}>:-Werror>
    )
endif()

# The game loads resources/ relative to the working directory: copy it next to the executable
add_custom_command(TARGET MultiplayerGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:MultiplayerGame>/resources"
)

# Visual Studio: start MultiplayerGame by default, from the executable's folder
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT MultiplayerGame)
set_target_properties(MultiplayerGame PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MultiplayerGame>"
)
