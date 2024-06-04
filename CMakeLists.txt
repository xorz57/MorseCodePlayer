cmake_minimum_required(VERSION 3.26)

cmake_policy(SET CMP0091 NEW)

project(MorseCodePlayer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(VCPKG_TARGET_TRIPLET)
    if(MSVC)
        if (VCPKG_TARGET_TRIPLET MATCHES "-static$")
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        else()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        endif()
    endif()
endif()

find_package(OpenAL REQUIRED)

add_executable(MorseCodePlayer src/Main.cpp)

target_link_libraries(MorseCodePlayer PRIVATE OpenAL::OpenAL)
