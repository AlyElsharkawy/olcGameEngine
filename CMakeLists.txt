# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.5)

# Set the project name and the C++ standard
project(3DEngineRebornRelease)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

# Setting output lib and bin directories for debug and release mode 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../lib/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/$<CONFIG>")

#This is for my main headers
include_directories("./include")
#This is for all of the OLC stuff
include_directories("./include/OLCHeaders/")
#This is for third party libraries 
include_directories("./include/External/")
#This is for the user's header files when they are developing a game
include_directories("./user/include/")
#For Dear ImGUI
include_directories("./include/External/ImGUI/")

option(ENABLE_SANITIZERS "Enable sanitizers (ASAN + UBSAN)" OFF)

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
find_package(yaml-cpp REQUIRED)

# We will create a shared library for each .cpp and .h file we have
# We will then create debug and release configs for them
add_library(gui_essentials STATIC ./src/guiEssentials.cpp)
add_library(misc_functions STATIC ./src/miscFunctions.cpp ./src/stbImageWriteImplementation.cpp)
add_library(clipping STATIC ./src/clippingRoutines.cpp)
add_library(essential STATIC ./src/essentialFunctions.cpp)
add_library(mesh STATIC ./src/meshMathEssentials.cpp)
add_library(geometry STATIC ./src/geometricPrimitives.cpp)
add_library(matrix STATIC ./src/matrixMathEssentials.cpp)
add_library(normal STATIC ./src/normalMathEssentials.cpp)
add_library(vector STATIC ./src/vectorMathEssentials.cpp)
add_library(triangle STATIC ./src/triangleMathEssentials.cpp)
add_library(misc_primitives STATIC ./src/miscPrimitives.cpp)
add_library(input STATIC ./src/inputManager.cpp)
add_library(audio STATIC ./src/audioManager.cpp)
add_library(im_gui STATIC
./src/External/ImGUI/imgui.cpp
./src/External/ImGUI/imgui_demo.cpp
./src/External/ImGUI/imgui_draw.cpp
./src/External/ImGUI/imgui_widgets.cpp
./src/External/ImGUI/imgui_impl_opengl3.cpp
./src/External/ImGUI/imgui_impl_opengl2.cpp
./src/External/ImGUI/imgui_tables.cpp
)
add_library(drawing STATIC ./src/drawingRoutines.cpp)
add_library(serialization STATIC ./src/serialization/lightSerialization.cpp ./src/serialization/meshSerialization.cpp)
target_link_libraries(serialization PRIVATE yaml-cpp)
target_link_libraries(geometry PRIVATE misc_functions)
target_link_libraries(misc_functions PRIVATE misc_primitives)
target_link_libraries(misc_primitives PRIVATE vector)
target_link_libraries(misc_primitives PRIVATE matrix geometry)
target_link_libraries(clipping PRIVATE geometry)
target_link_libraries(normal PRIVATE vector)
target_link_libraries(essential PRIVATE drawing)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(OBJFileSerializer ./src/extra/OBJFileSerializer.cpp)
add_executable(GUITest ./src/extra/GUITest.cpp)
add_executable(TestApp ./src/extra/test.cpp)
add_executable(PART_ONE_FINAL ./src/extra/Part1Final.cpp)
add_executable(PART_TWO_FINAL ./src/extra/Part2Final.cpp)
add_executable(PART_THREE_FINAL ./src/extra/Part3Final.cpp)
add_executable(PART_FOUR_FINAL ./src/extra/Part4Final.cpp)
add_executable(OLC_GUI_EXAMPLE ./src/extra/Examples/TEST_QuickGUI.cpp)
add_executable(OLC_CUSTOM_FONT ./src/extra/Examples/Example.cpp)
add_executable(IM_GUI_TEST ./src/extra/ImGUITest.cpp)
add_executable(SERIALIZATION_TEST ./src/extra/serializationTest.cpp)

#Additional libraries for each project 
set(PROJECT_NAME_LIBS
  essential
  misc_functions
  geometry
  matrix
  normal
  vector
  triangle
  clipping
  gui_essentials
  misc_primitives
  input
  audio
  serialization
  im_gui
)

set(GUI_TEST_LIBS
  misc_functions
  gui_essentials
)

set(OBJ_FILE_SERIALIZER_LIBS
  geometry
  misc_functions
)

set(PART_FOUR_FINAL_LIBS
  misc_functions
)

set(TEST_APP_LIBS
  matrix
  geometry
  vector
  normal
)

set(OLC_CUSTOM_FONT_LIBS
  misc_functions
)

if (CMAKE_BUILD_TYPE STREQUAL "release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_RELEASE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_DEBUG)
endif()

if (ENABLE_SANITIZERS)
    message(STATUS "Sanitizers ENABLED")

    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address,undefined
    )
else()
    message(STATUS "Sanitizers DISABLED")
endif()


# For Linux and windows support
if(UNIX)
    set(PLATFORM_LIBRARIES X11 GL pthread stdc++fs png ${X11_LIBRARIES})
elseif(WIN32)
    set(PLATFORM_LIBRARIES user32 gdi32 opengl32 gdiplus Shlwapi dwmapi stdc++fs)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Link the necessary libraries
#linking with stdc++fs is not required if your compiler is above version 9.1
target_link_libraries(${PROJECT_NAME} PRIVATE ${PLATFORM_LIBRARIES} ${PROJECT_NAME_LIBS} freetype)
target_link_libraries(OBJFileSerializer PRIVATE ${PLATFORM_LIBRARIES} ${OBJ_FILE_SERIALIZER_LIBS})
target_link_libraries(TestApp PRIVATE ${PLATFORM_LIBRARIES} ${TEST_APP_LIBS})
target_link_libraries(GUITest PRIVATE ${PLATFORM_LIBRARIES} ${GUI_TEST_LIBS} freetype)
target_link_libraries(PART_ONE_FINAL PRIVATE ${PLATFORM_LIBRARIES} )
target_link_libraries(PART_TWO_FINAL PRIVATE ${PLATFORM_LIBRARIES})
target_link_libraries(PART_THREE_FINAL PRIVATE ${PLATFORM_LIBRARIES})
target_link_libraries(PART_FOUR_FINAL PRIVATE ${PLATFORM_LIBRARIES} ${PART_FOUR_FINAL_LIBS})
target_link_libraries(OLC_GUI_EXAMPLE PRIVATE ${PLATFORM_LIBRARIES} ${OLC_GUI_EXAMPLE} freetype)
target_link_libraries(OLC_CUSTOM_FONT PRIVATE ${PLATFORM_LIBRARIES} ${OLC_CUSTOM_FONT_LIBS})
target_link_libraries(IM_GUI_TEST PRIVATE ${PLATFORM_LIBRARIES} im_gui)
target_link_libraries(SERIALIZATION_TEST PRIVATE essential ${PLATFORM_LIBRARIES} misc_functions serialization)

add_compile_options(
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2 -s>
)

add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
  COMMENT "Building debug configuration"
)

add_custom_target(debug-san
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
  COMMENT "Building debug configuration with sanitizers"
)

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release
  COMMENT "Building release configuration"
)
