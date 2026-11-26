cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_VERSION "10.0.26100.0" CACHE STRING "Windows SDK version")
project(TheFear LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(GAME_DATA "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(GAME_SHADER "${CMAKE_CURRENT_SOURCE_DIR}/shader.hlsl")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/Private/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${SOURCE_DIR}/Public/*.h" "${SOURCE_DIR}/Public/*.hpp"
    "${SOURCE_DIR}/Private/*.h" "${SOURCE_DIR}/Private/*.hpp"
)
source_group(TREE "${SOURCE_DIR}" FILES ${SOURCES} ${HEADERS})
source_group("Shaders" FILES "${GAME_SHADER}")
# The renderer compiles both shader entry points at runtime.
set_source_files_properties("${GAME_SHADER}" PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} "${GAME_SHADER}")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${SOURCE_DIR}/Public"
    "${SOURCE_DIR}/Private"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d11 dxgi dxguid d3dcompiler
    windowscodecs xaudio2
    dinput8 winmm gdi32 ole32
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _MBCS NOMINMAX WIN32_LEAN_AND_MEAN
    $<$<CONFIG:Debug>:_DEBUG>
)
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # The existing Win32 and model-loader APIs take narrow, mutable strings.
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:strictStrings-)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

add_custom_target(CopyGameData ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_DATA}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAME_SHADER}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shader.hlsl"
    DEPENDS "${GAME_SHADER}"
    COMMENT "Copying game assets and shader.hlsl"
    VERBATIM
)
add_dependencies(${PROJECT_NAME} CopyGameData)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY "${GAME_DATA}" DESTINATION . PATTERN "Thumbs.db" EXCLUDE)
install(FILES "${GAME_SHADER}" DESTINATION .)
