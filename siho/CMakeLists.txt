file(GLOB_RECURSE HEADER_FILES "src/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Group the files according to their actual paths
foreach(FILE IN LISTS HEADER_FILES SOURCE_FILES)
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" GROUP "${PARENT_DIR}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_library(siho STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(siho PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(siho PUBLIC
    volk
    glm
    imgui
    spdlog
    vma
    glfw
)

if(WIN32)
    target_compile_definitions(siho PRIVATE
        SH_PLATFORM_WINDOWS
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    GLFW_INCLUDE_NONE
    $<$<CONFIG:Debug>:SH_DEBUG>
)
