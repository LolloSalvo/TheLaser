cmake_minimum_required(VERSION 3.16)
project(TheLaser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/W3 /permissive-)
else()
    add_compile_options(-Wall -Wno-unused-parameter)
endif()

if(WIN32)
    # On Windows, prefer the bundled SDL2 distributions under Libraries/
    set(SDL2_DIR        "${CMAKE_SOURCE_DIR}/Libraries/SDLMain/SDL2-2.30.11")
    set(SDL2_IMAGE_DIR  "${CMAKE_SOURCE_DIR}/Libraries/SDLImage/SDL2_image-2.8.4")
    set(SDL2_MIXER_DIR  "${CMAKE_SOURCE_DIR}/Libraries/SDLMixer/SDL2_mixer-2.8.0")
    set(SDL2_TTF_DIR    "${CMAKE_SOURCE_DIR}/Libraries/SDLTtf/SDL2_ttf-2.24.0")

    set(SDL2_INCLUDE_DIRS
        "${SDL2_DIR}/include"
        "${SDL2_IMAGE_DIR}/include"
        "${SDL2_MIXER_DIR}/include"
        "${SDL2_TTF_DIR}/include"
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_arch x64)
    else()
        set(_arch x86)
    endif()

    set(SDL2_LIBRARIES
        "${SDL2_DIR}/lib/${_arch}/SDL2.lib"
        "${SDL2_DIR}/lib/${_arch}/SDL2main.lib"
        "${SDL2_IMAGE_DIR}/lib/${_arch}/SDL2_image.lib"
        "${SDL2_MIXER_DIR}/lib/${_arch}/SDL2_mixer.lib"
        "${SDL2_TTF_DIR}/lib/${_arch}/SDL2_ttf.lib"
        opengl32
    )

    set(SDL2_RUNTIME_DLL_DIRS
        "${SDL2_DIR}/lib/${_arch}"
        "${SDL2_IMAGE_DIR}/lib/${_arch}"
        "${SDL2_MIXER_DIR}/lib/${_arch}"
        "${SDL2_TTF_DIR}/lib/${_arch}"
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
    pkg_check_modules(SDL2_MIXER REQUIRED IMPORTED_TARGET SDL2_mixer)
    pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(Engine)
add_subdirectory(TheLaser)
