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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/$<CONFIG>")

#This is for my main headers
include_directories("./include")
#This is for all of the OLC stuff
include_directories("./include/OLCHeaders/")
#This is for third party libaries 
include_directories("./include/External/")
#This is for the user's header files when they are developing a game
include_directories("./user/include/")


find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
find_package(yaml-cpp REQUIRED)

# We will create a shared library for each .cpp and .h file we have
# We will then create debug and release configs for them
add_library(GUI_ESSENTIALS_LIB SHARED ./src/guiEssentials.cpp)
add_library(MISC_FUNCTIONS_LIB SHARED ./src/miscFunctions.cpp ./src/stbImageWriteImplementation.cpp)
add_library(CLIPPING_ROUTINES_LIB SHARED ./src/clippingRoutines.cpp)
add_library(ESSENTIAL_FUNCTIONS_LIB SHARED ./src/essentialFunctions.cpp)
add_library(MESH_MATH_LIB SHARED ./src/meshMathEssentials.cpp)
add_library(GEOMETRY_LIB SHARED ./src/geometricPrimitives.cpp)
add_library(MATRIX_MATH_LIB SHARED ./src/matrixMathEssentials.cpp)
add_library(NORMAL_MATH_LIB SHARED ./src/normalMathEssentials.cpp)
add_library(VECTOR_MATH_LIB SHARED ./src/vectorMathEssentials.cpp)
add_library(TRIANGLE_MATH_LIB SHARED ./src/triangleMathEssentials.cpp)
add_library(MISC_PRIMITIVES_LIB SHARED ./src/miscPrimitives.cpp)
add_library(INPUT_MNGR_LIB SHARED ./src/inputManager.cpp)
add_library(AUDIO_LIB SHARED ./src/audioManager.cpp)
add_library(PLATFORM_LIB SHARED ./src/platformFunctions.cpp)
target_link_libraries(GEOMETRY_LIB PRIVATE MISC_FUNCTIONS_LIB)
target_link_libraries(PLATFORM_LIB PRIVATE ${XFIXES_LIB})

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

#Additional libraries for each project 
set(PROJECT_NAME_LIBS
  ESSENTIAL_FUNCTIONS_LIB
  MISC_FUNCTIONS_LIB
  GEOMETRY_LIB
  MATRIX_MATH_LIB
  NORMAL_MATH_LIB
  VECTOR_MATH_LIB
  TRIANGLE_MATH_LIB
  CLIPPING_ROUTINES_LIB
  GUI_ESSENTIALS_LIB
  MISC_PRIMITIVES_LIB
  INPUT_MNGR_LIB
  AUDIO_LIB
  PLATFORM_LIB
)

set(GUI_TEST_LIBS
  MISC_FUNCTIONS_LIB
  GUI_ESSENTIALS_LIB
)

set(OBJ_FILE_SERIALIZER_LIBS
  GEOMETRY_LIB
  MISC_FUNCTIONS_LIB
)

set(PART_FOUR_FINAL_LIBS
  MISC_FUNCTIONS_LIB
)

set(TEST_APP_LIBS
  MATRIX_MATH_LIB
  GEOMETRY_LIB
  VECTOR_MATH_LIB
  NORMAL_MATH_LIB
)

set(OLC_CUSTOM_FONT_LIBS
  MISC_FUNCTIONS_LIB
)

# For Linux and windows support
if(UNIX)
    set(PLATFORM_LIBRARIES X11 GL pthread stdc++fs ${X11_LIBRARIES})
elseif(WIN32)
    set(PLATFORM_LIBRARIES user32 gdi32 opengl32 gdiplus Shlwapi dwmapi stdc++fs)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# Link the necessary libraries
#linking with stdc++fs is not required if your compiler is above version 9.1
target_link_libraries(${PROJECT_NAME} PRIVATE ${PLATFORM_LIBRARIES} ${PROJECT_NAME_LIBS} freetype
yaml-cpp)
target_link_libraries(OBJFileSerializer PRIVATE ${PLATFORM_LIBRARIES} ${OBJ_FILE_SERIALIZER_LIBS})
target_link_libraries(TestApp PRIVATE ${PLATFORM_LIBRARIES} ${TEST_APP_LIBS})
target_link_libraries(GUITest PRIVATE ${PLATFORM_LIBRARIES} ${GUI_TEST_LIBS} freetype)
target_link_libraries(PART_ONE_FINAL PRIVATE ${PLATFORM_LIBRARIES} )
target_link_libraries(PART_TWO_FINAL PRIVATE ${PLATFORM_LIBRARIES})
target_link_libraries(PART_THREE_FINAL PRIVATE ${PLATFORM_LIBRARIES})
target_link_libraries(PART_FOUR_FINAL PRIVATE ${PLATFORM_LIBRARIES} ${PART_FOUR_FINAL_LIBS})
target_link_libraries(OLC_GUI_EXAMPLE PRIVATE ${PLATFORM_LIBRARIES} ${OLC_GUI_EXAMPLE} freetype)
target_link_libraries(OLC_CUSTOM_FONT PRIVATE ${PLATFORM_LIBRARIES} ${OLC_CUSTOM_FONT_LIBS})

add_compile_options(
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2 -s>
)

add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
  COMMENT "Building debug configuration"
)

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release
  COMMENT "Building release configuration"
)
