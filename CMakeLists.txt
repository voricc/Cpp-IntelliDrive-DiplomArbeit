cmake_minimum_required(VERSION 3.5.0)
project(IntelliDrive VERSION 0.1.0)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)
# Build SFML
add_subdirectory(vendors/sfml/)

# Collect all the source and header files
file(GLOB ALL_SOURCE_FILES src/*.cpp)
file(GLOB ALL_HEADER_FILES include/*.hpp)

# Add the main executable target
add_executable(${PROJECT_NAME} ${ALL_SOURCE_FILES} ${ALL_HEADER_FILES}
        src/CarChooseState.cpp
        include/CarChooseState.h
        src/LevelCreator.cpp
        include/LevelCreator.h
        src/LevelSelectState.cpp
        include/LevelSelectState.h
        include/Tile.h
        src/Tile.cpp
        src/GameState.cpp
        src/AiGameState.cpp
        include/AiGameState.h
        src/SettingsState.cpp
        include/SettingsState.h
        include/SettingsManager.h
        include/MetricsManager.h
        src/SettingsManager.cpp
        src/MetricsManager.cpp
        src/DropDown.cpp
        include/DropDown.h
)

# Use C++17 standards
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Include directories for SFML and Box2D
target_include_directories(
        ${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/include/
        PRIVATE ${PROJECT_SOURCE_DIR}/vendors/sfml/include/
        PRIVATE ${PROJECT_SOURCE_DIR}/vendors/box2d/include/  # Include Box2D headers
)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Link Box2D static library
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/vendors/bin3/libbox2d.a)

# Define the resource directory
set(RESOURCE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)

# Create the resource directory if it doesn't exist
file(MAKE_DIRECTORY ${RESOURCE_DIR})

# Get all files in the resources directory
file(GLOB RESOURCE_FILES ${PROJECT_SOURCE_DIR}/resources/*)

# Add custom command to copy resource files
foreach(resource ${RESOURCE_FILES})
    get_filename_component(resource_name ${resource} NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${resource} ${RESOURCE_DIR}/${resource_name}
            COMMENT "Copying ${resource} to ${RESOURCE_DIR}/${resource_name}"
    )
endforeach()

# Copy necessary DLLs after the build
if (WIN32)
    # Define the path to the SFML and MinGW (GCC) DLLs
    set(SFML_DLLS_DIR ${PROJECT_SOURCE_DIR}/vendors/bin)
    set(GCC_DLLS_DIR ${PROJECT_SOURCE_DIR}/vendors/bin2)

    # List of required DLLs
    set(SFML_DLLS
            "${SFML_DLLS_DIR}/sfml-graphics-2.dll"
            "${SFML_DLLS_DIR}/sfml-window-2.dll"
            "${SFML_DLLS_DIR}/sfml-system-2.dll"
    )

    set(GCC_DLLS
            "${GCC_DLLS_DIR}/libgcc_s_seh-1.dll"
            "${GCC_DLLS_DIR}/libstdc++-6.dll"
            "${GCC_DLLS_DIR}/libwinpthread-1.dll"
    )

    # Add custom command to copy SFML DLLs
    foreach(dll ${SFML_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMENT "Copying SFML DLL ${dll} to output directory"
        )
    endforeach()

    # Add custom command to copy GCC DLLs
    foreach(dll ${GCC_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMENT "Copying GCC DLL ${dll} to output directory"
        )
    endforeach()

endif()

# Copy the Box2D static library (.a) to the output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/vendors/bin3/libbox2d.a ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Copying Box2D static library to output directory"
)
