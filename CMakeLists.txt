cmake_minimum_required(VERSION 3.16)

project(HelloCompGraph)

file(GLOB_RECURSE COMMON_SRC include/*.h src/*.cpp)
add_library(COMMON STATIC ${COMMON_SRC})
target_include_directories(COMMON PUBLIC "include/")
target_compile_definitions(COMMON PUBLIC IMGUI_IMPL_OPENGL_ES3)
target_compile_definitions(COMMON PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC SDL2::SDL2 SDL2::SDL2main)

find_package(GLEW REQUIRED)
target_link_libraries(COMMON PUBLIC GLEW::GLEW)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC assimp::assimp)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC imgui::imgui)

find_package(glm CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC glm)

#Compile and copy shaders
if(MSVC)
    if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
    else()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
    endif()
elseif(UNIX)
    set(GLSL_VALIDATOR "glslangValidator")
endif()

find_program(GLSL_VALIDATOR_FOUND ${GLSL_VALIDATOR})
IF(NOT GLSL_VALIDATOR_FOUND)
    MESSAGE(FATAL_ERROR "Please install VulkanSDK and put it in path (current path: $ENV{VULKAN_SDK})")
ENDIF()

file(GLOB_RECURSE shader_files shaders/*.vert shaders/*.frag)
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/shaders")
foreach(SHADER ${shader_files})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    get_filename_component(PATH_NAME ${SHADER} DIRECTORY)
    get_filename_component(EXTENSION ${SHADER} EXT)
    file(RELATIVE_PATH PATH_NAME "${PROJECT_SOURCE_DIR}" ${PATH_NAME})
    set(SHADER_OUTPUT "${PROJECT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")

    add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            DEPENDS ${SHADER}
            DEPENDS
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/${PATH_NAME}"
            COMMAND ${GLSL_VALIDATOR} ${SHADER}
            COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} "${SHADER_OUTPUT}"
    )
    list(APPEND SHADER_BINARY_FILES ${SHADER_OUTPUT})
endforeach(SHADER)

add_custom_target(
        ShaderTarget
        DEPENDS ${SHADER_BINARY_FILES} ${shader_files})

file(GLOB main_projects "main/*.cpp")

foreach(main_project_path ${main_projects} )
    # I used a simple string replace, to cut off .cpp.
    get_filename_component(main_project_name ${main_project_path} NAME_WLE )
    file(GLOB shader_files shaders/${main_project_name}/*)
    add_executable(${main_project_name} ${main_project_path} ${shader_files})
    target_link_libraries(${main_project_name} COMMON)
    add_dependencies(${main_project_name} ShaderTarget)

endforeach()