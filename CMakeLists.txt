cmake_minimum_required(VERSION 3.15...3.16 FATAL_ERROR)

project(pygmentize_cache VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)


# Create executable target
# (adding headers here only so that they show up in the IDE)
add_executable(pygmentize_cache
    include_3rdparty/json.hpp
    include_3rdparty/picosha2.hpp
    src/config.hpp
    src/fileutil.hpp
    src/proclauncher.hpp
    src/envvar.hpp
    src/pygmentize_cache.cpp
)


# Platform specific code
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(pygmentize_cache PUBLIC 
        src/envvar_windows.cpp
        src/proclauncher_windows.cpp
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_sources(pygmentize_cache PUBLIC 
        src/envvar_linux.cpp
        src/proclauncher_linux.cpp
    )
else()
    message(FATAL_ERROR "Unsupported platform." )
endif()


#Set target properties
target_include_directories(pygmentize_cache
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_3rdparty>
)


target_compile_options(pygmentize_cache PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>)

    
# Link MSVC runtime library statically, to ease deployment
set_property(TARGET pygmentize_cache PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  
  
# Set MSVC startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "pygmentize_cache")

