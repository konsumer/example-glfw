cmake_minimum_required(VERSION 3.12)
project(example)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp)

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

if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-Oz -sUSE_GLFW=3 -sSINGLE_FILE"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs/"
    SUFFIX ".mjs"
  )
else()
  add_compile_definitions(${PROJECT_NAME} -DGL_SILENCE_DEPRECATION)
  find_package(OpenGL REQUIRED)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

  include(FetchContent)
  FetchContent_Declare(glfw URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip)
  FetchContent_MakeAvailable(glfw)

  set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
  )
  target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_LIBRARIES})
endif()
