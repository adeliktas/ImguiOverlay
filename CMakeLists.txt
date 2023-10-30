cmake_minimum_required(VERSION 3.1)

project(ImguiOverlay)

set(ImguiOverlay_INCLUDES src/include)
include_directories(${ImguiOverlay_INCLUDES})

set(IMGUI_DIR external/imgui) #https://github.com/ocornut/imgui/tree/master
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)
set(SOURCES
    src/main.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

set(OBJS "")
foreach(source ${SOURCES})
    get_filename_component(name ${source} NAME_WE)
    add_library(${name} OBJECT ${source})
    list(APPEND OBJS $<TARGET_OBJECTS:${name}>)
endforeach()

# Options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CXXFLAGS "-Wall -Wformat -g")
set(CXXFLAGS "${CXXFLAGS} -I${IMGUI_DIR} -I${IMGUI_DIR}/backends")
set(CXXFLAGS "${CXXFLAGS} -g") #debugging info
set(CXXFLAGS "${CXXFLAGS} -DIMGUI_IMPL_OPENGL_ES2")
set(LINUX_GL_LIBS "-lGL")
set(LIBS "${LINUX_GL_LIBS} -lX11")
find_package(PkgConfig REQUIRED)

set(GLFW_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../devreqs/glfw/include/GLFW)
find_package(glfw3 REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

set(LIBS "${LIBS} -l${GLFW_LIBRARIES}")
set(CXXFLAGS "${CXXFLAGS} ${GLFW_CFLAGS}")

set(LIBS "${LIBS} -lm -ldl -lpthread")

include_directories(${CMAKE_SOURCE_DIR}/src/include)
#link_directories(${CMAKE_SOURCE_DIR}/mylib)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${OBJS})
target_compile_options(${PROJECT_NAME} PRIVATE ${CXXFLAGS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "CustomControls")
