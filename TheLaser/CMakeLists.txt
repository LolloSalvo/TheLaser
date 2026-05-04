add_executable(TheLaser
    Cell.cpp
    Digit.cpp
    Game.cpp
    Grid.cpp
    Laser.cpp
    LevelGenerator.cpp
    LightBeam.cpp
    main.cpp
    Mirror.cpp
    pch.cpp
)

target_link_libraries(TheLaser PRIVATE Engine)

# Copy resources next to the executable so SDL_image/SDL_ttf can find them.
add_custom_command(TARGET TheLaser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:TheLaser>"
    COMMENT "Copying Resources/ next to executable"
)

if(WIN32)
    foreach(_dir IN LISTS SDL2_RUNTIME_DLL_DIRS)
        add_custom_command(TARGET TheLaser POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_dir}" "$<TARGET_FILE_DIR:TheLaser>"
            COMMENT "Copying SDL2 DLLs from ${_dir}"
        )
    endforeach()
endif()
