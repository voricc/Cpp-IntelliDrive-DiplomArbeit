cmake_minimum_required(VERSION 3.26)
project(IntelliDrive VERSION 0.1.0)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)

# Stellen Sie sicher, dass SFML als Shared Libraries gebaut wird
set(SFML_BUILD_SHARED_LIBS TRUE CACHE BOOL "Build SFML as shared libraries" FORCE)

# Build SFML
add_subdirectory(vendors/sfml/ vendors/sfml_build/)

# Explicitly list all source files
set(SOURCE_FILES
        src/main.cpp
        src/CarChooseState.cpp
        src/LevelCreator.cpp
        src/LevelSelectState.cpp
        src/Tile.cpp
        src/GameState.cpp
        src/AiGameState.cpp
        src/SettingsState.cpp
        src/SettingsManager.cpp
        src/MetricsManager.cpp
        src/DropDown.cpp
        src/Game.cpp           # Hinzugefügt
        src/Car.cpp            # Hinzugefügt
        src/ResourceManager.cpp # Hinzugefügt
        src/MenuState.cpp      # Hinzugefügt
        src/PauseState.cpp     # Hinzugefügt
        src/DeathState.cpp     # Hinzugefügt
        # Füge hier weitere .cpp-Dateien hinzu, falls vorhanden
)

# Optional: Liste aller Header-Dateien auf (kann weggelassen werden)
set(HEADER_FILES
        include/CarChooseState.h
        include/LevelCreator.h
        include/LevelSelectState.h
        include/Tile.h
        include/AiGameState.h
        include/SettingsState.h
        include/SettingsManager.h
        include/MetricsManager.h
        include/DropDown.h
        include/Game.h
        include/Car.h
        include/ResourceManager.h
        include/MenuState.h     # Hinzugefügt
        include/PauseState.h    # Hinzugefügt
        include/DeathState.h    # Hinzugefügt
        # Füge hier weitere .h/.hpp-Dateien hinzu, falls vorhanden
)

# Add the main executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Include directories for SFML
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include/
        ${PROJECT_SOURCE_DIR}/vendors/sfml/include/
        # Falls du andere Abhängigkeiten hast, füge sie hier hinzu
)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        # Falls du weitere SFML-Module oder andere Bibliotheken verwendest, füge sie hier hinzu
)

# Define the resource directory
set(RESOURCE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)

# Create the resource directory if it doesn't exist
file(MAKE_DIRECTORY ${RESOURCE_DIR})

# Get all files in the resources directory
file(GLOB RESOURCE_FILES "${PROJECT_SOURCE_DIR}/resources/*")

# Add custom command to copy resource files
foreach(resource ${RESOURCE_FILES})
    get_filename_component(resource_name ${resource} NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${resource}" "${RESOURCE_DIR}/${resource_name}"
            COMMENT "Copying ${resource} to ${RESOURCE_DIR}/${resource_name}"
    )
endforeach()

# Copy necessary SFML DLLs after the build (für Visual Studio)
if (WIN32)
    # Verwende Generator Expressions, um die korrekten Pfade basierend auf der Build-Konfiguration zu referenzieren
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:sfml-graphics>"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sfml-graphics.dll"
            COMMENT "Copying SFML DLL sfml-graphics.dll to output directory"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:sfml-window>"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sfml-window.dll"
            COMMENT "Copying SFML DLL sfml-window.dll to output directory"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:sfml-system>"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sfml-system.dll"
            COMMENT "Copying SFML DLL sfml-system.dll to output directory"
    )
endif()

# Optional: Set properties for Visual Studio to use multithreaded DLL runtime
if (MSVC)
    # Set runtime library to Multithreaded DLL (/MD)
    foreach(flag_var
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
    endforeach()
endif()
