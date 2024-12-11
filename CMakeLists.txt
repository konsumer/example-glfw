cmake_minimum_required(VERSION 3.12)
project(example)
set(CMAKE_CXX_STANDARD 23)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
  set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
endif()

# enable colors for ninja
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_BUILD_TYPE Debug)

function(SETUP_GLFW_TARGET name)
  if(EMSCRIPTEN)
    set_target_properties(${name} PROPERTIES
      LINK_FLAGS "-Oz -sUSE_GLFW=3 -sSINGLE_FILE"
      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs/"
      SUFFIX ".mjs"
    )
  else()
    add_compile_definitions(${name} -DGL_SILENCE_DEPRECATION)
    find_package(OpenGL REQUIRED)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    include(FetchContent)
    FetchContent_Declare(glfw URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip)
    FetchContent_MakeAvailable(glfw)

    set_target_properties(${name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
    target_link_libraries(${name} glfw ${OPENGL_LIBRARIES})
  endif()
endfunction()

add_executable(example src/example.cpp)
SETUP_GLFW_TARGET(example)

add_executable(jstest src/jstest.c)
SETUP_GLFW_TARGET(jstest)
