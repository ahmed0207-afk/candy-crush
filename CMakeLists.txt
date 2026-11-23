cmake_minimum_required(VERSION 3.24)
project(CandyCrushStarter LANGUAGES CXX)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(SFML
    URL https://codeload.github.com/SFML/SFML/tar.gz/refs/tags/3.0.2
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL_HASH SHA256=0034e05f95509e5d3fb81b1625713e06da7b068f210288ce3fd67106f8f46995
)
FetchContent_MakeAvailable(SFML)

add_executable(CandyCrushStarter main.cpp)
target_compile_features(CandyCrushStarter PRIVATE cxx_std_17)
target_link_libraries(CandyCrushStarter PRIVATE SFML::Graphics)
set_target_properties(CandyCrushStarter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Keep the bundled font beside the executable for terminal and IDE launches.
add_custom_command(TARGET CandyCrushStarter POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:CandyCrushStarter>/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/level.txt"
        "$<TARGET_FILE_DIR:CandyCrushStarter>/level.txt"
    VERBATIM
)
