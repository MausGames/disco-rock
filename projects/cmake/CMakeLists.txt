cmake_minimum_required(VERSION 3.19)
set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..)


project(DiscoRock VERSION 1.3.0)
add_subdirectory(${BASE_DIRECTORY}/../CoreEngine/projects/cmake CoreEngine)
add_executable(DiscoRock)


set_target_properties(
    DiscoRock PROPERTIES
    C_STANDARD 11
    CXX_STANDARD 20
    DEBUG_POSTFIX _debug
    SUFFIX .elf
    RUNTIME_OUTPUT_DIRECTORY ${BASE_DIRECTORY}/executable/bin/linux_x86_64/
)


target_sources(
    DiscoRock PRIVATE
    ${BASE_DIRECTORY}/../GameJoltAPI/source/Base64.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/MD5.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/gjAPI.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/gjDataItem.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/gjNetwork.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/gjScore.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/gjTrophy.cpp
    ${BASE_DIRECTORY}/../GameJoltAPI/source/gjUser.cpp
    ${BASE_DIRECTORY}/source/cBackground.cpp
    ${BASE_DIRECTORY}/source/cBeverage.cpp
    ${BASE_DIRECTORY}/source/cCombatText.cpp
    ${BASE_DIRECTORY}/source/cFirst.cpp
    ${BASE_DIRECTORY}/source/cGame.cpp
    ${BASE_DIRECTORY}/source/cInterface.cpp
    ${BASE_DIRECTORY}/source/cMenu.cpp
    ${BASE_DIRECTORY}/source/cOnline.cpp
    ${BASE_DIRECTORY}/source/cPlate.cpp
    ${BASE_DIRECTORY}/source/cRay.cpp
    ${BASE_DIRECTORY}/source/cRock.cpp
    ${BASE_DIRECTORY}/source/cStage.cpp
    ${BASE_DIRECTORY}/source/cTrap.cpp
    ${BASE_DIRECTORY}/source/application.cpp
    ${BASE_DIRECTORY}/source/main.cpp
)


target_include_directories(
    DiscoRock PRIVATE
    ${BASE_DIRECTORY}/source
    ${BASE_DIRECTORY}/../GameJoltAPI/source
    ${BASE_DIRECTORY}/../GameJoltAPI/libraries/include
)


target_precompile_headers(
   DiscoRock PRIVATE
   $<$<COMPILE_LANGUAGE:CXX>: ${BASE_DIRECTORY}/source/main.h>
)


target_link_libraries(
    DiscoRock PRIVATE
    CoreEngine -l:libcurl.so.4
)