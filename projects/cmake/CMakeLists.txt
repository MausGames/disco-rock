cmake_minimum_required(VERSION 3.31)

set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(ENGINE_DIRECTORY ${BASE_DIRECTORY}/../core-engine)

project(DiscoRock VERSION 1.3.0)
add_subdirectory(${ENGINE_DIRECTORY}/projects/cmake CoreEngine)
add_executable(DiscoRock)

set_target_properties(
    DiscoRock PROPERTIES
    C_STANDARD 17
    CXX_STANDARD 23
    CXX_SCAN_FOR_MODULES OFF
    DEBUG_POSTFIX _debug
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH $ORIGIN
)

target_sources(
    DiscoRock PRIVATE
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/Base64.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/MD5.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/gjAPI.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/gjDataItem.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/gjNetwork.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/gjScore.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/gjTrophy.cpp
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source/gjUser.cpp
    ${BASE_DIRECTORY}/source/Background.cpp
    ${BASE_DIRECTORY}/source/Beverage.cpp
    ${BASE_DIRECTORY}/source/CombatText.cpp
    ${BASE_DIRECTORY}/source/First.cpp
    ${BASE_DIRECTORY}/source/Game.cpp
    ${BASE_DIRECTORY}/source/Interface.cpp
    ${BASE_DIRECTORY}/source/Menu.cpp
    ${BASE_DIRECTORY}/source/Online.cpp
    ${BASE_DIRECTORY}/source/Plate.cpp
    ${BASE_DIRECTORY}/source/Ray.cpp
    ${BASE_DIRECTORY}/source/Rock.cpp
    ${BASE_DIRECTORY}/source/Stage.cpp
    ${BASE_DIRECTORY}/source/Trap.cpp
    ${BASE_DIRECTORY}/source/application.cpp
    ${BASE_DIRECTORY}/source/main.cpp
)

target_include_directories(
    DiscoRock PRIVATE
    ${BASE_DIRECTORY}/source
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/source
    ${BASE_DIRECTORY}/../game-jolt-api-cpp-library/libraries/include
)

target_precompile_headers(
   DiscoRock PRIVATE
   $<$<COMPILE_LANGUAGE:CXX>: ${BASE_DIRECTORY}/source/main.h>
)

target_link_libraries(
    DiscoRock PRIVATE
    CoreEngine
)

if(CORE_EMSCRIPTEN)

    set_target_properties(
        DiscoRock PROPERTIES
        SUFFIX .html
        RUNTIME_OUTPUT_DIRECTORY ${BASE_DIRECTORY}/executable/bin/emscripten
    )

else()

    set_target_properties(
        DiscoRock PROPERTIES
        SUFFIX .elf
        RUNTIME_OUTPUT_DIRECTORY ${BASE_DIRECTORY}/executable/bin/linux_x86_64
    )

    target_link_libraries(
        DiscoRock PRIVATE
        -l:libcurl.so.4
    )

    if(NOT CORE_DEBUG)

        add_custom_command(
            TARGET DiscoRock POST_BUILD
            COMMAND ${ENGINE_DIRECTORY}/tools/scripts/extract_symbols.sh $<TARGET_FILE:DiscoRock>
        )

    endif()

endif()