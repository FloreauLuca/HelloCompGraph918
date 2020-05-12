cmake_minimum_required(VERSION 3.16)

project(HelloCompGraph)

file(GLOB_RECURSE COMMON_SRC include/*.h src/*.cpp)
add_library(COMMON STATIC ${COMMON_SRC})
target_include_directories(COMMON PUBLIC "include/")

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC SDL2::SDL2 SDL2::SDL2main)

find_package(GLEW REQUIRED)
target_link_libraries(COMMON PUBLIC GLEW::GLEW)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC assimp::assimp)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(COMMON PUBLIC imgui::imgui)

find_package(glm CONFIG REQUIRED)
target_link_libraries(COMMON PRIVATE glm)

file(GLOB main_projects "main/*")

foreach(main_project_path ${main_projects} )
    # I used a simple string replace, to cut off .cpp.
    get_filename_component(main_project_name ${main_project_path} NAME_WLE )
    add_executable(${main_project_name} ${main_project_path})
    target_link_libraries(${main_project_name} COMMON)
endforeach()