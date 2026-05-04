add_library(Engine STATIC
    BaseGame.cpp
    Matrix2x3.cpp
    SoundEffect.cpp
    SoundStream.cpp
    structs.cpp
    SVGParser.cpp
    Texture.cpp
    utils.cpp
    Vector2f.cpp
)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_include_directories(Engine PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(Engine PUBLIC ${SDL2_LIBRARIES})
else()
    target_link_libraries(Engine PUBLIC
        PkgConfig::SDL2
        PkgConfig::SDL2_IMAGE
        PkgConfig::SDL2_MIXER
        PkgConfig::SDL2_TTF
        OpenGL::GL
    )
endif()
