cmake_minimum_required(VERSION 3.20)
project(TheFear CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Legacy DirectX SDK (D3DX11CreateShaderResourceViewFromFile / D3DX11CompileFromFile için gerekli) ---
set(DXSDK_DIR "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DXSDK_LIB_DIR "${DXSDK_DIR}/Lib/x64")
else()
    set(DXSDK_LIB_DIR "${DXSDK_DIR}/Lib/x86")
endif()

# --- Kaynak dosyalar ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Source/Private/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Source/Public/*.h")

add_executable(TheFear WIN32 ${SOURCES} ${HEADERS})

target_include_directories(TheFear PRIVATE
    "${CMAKE_SOURCE_DIR}/Source/Public"
    "${CMAKE_SOURCE_DIR}/Source/Private"
    "${DXSDK_DIR}/Include"
)

target_link_directories(TheFear PRIVATE "${DXSDK_LIB_DIR}")

target_link_libraries(TheFear PRIVATE
    d3d11.lib
    dxgi.lib
    d3dcompiler.lib
    dinput8.lib
    dxguid.lib
    d3dx11.lib
)

target_compile_definitions(TheFear PRIVATE _MBCS)

# Solution Explorer'da fiziksel klasör yapısını yansıt
source_group(TREE "${CMAKE_SOURCE_DIR}/Source" FILES ${SOURCES} ${HEADERS})

# data/ ve shader.hlsl'i relative path ile bulabilmesi için çalışma dizinini repo köküne sabitle
set_target_properties(TheFear PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)