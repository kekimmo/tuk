
cmake_minimum_required(VERSION 2.6)

project(Tuk)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")

find_package(SDL)
find_package(OpenGL)
find_package(PNG)

link_libraries(
  m
  ${SDL_LIBRARY}
  ${OPENGL_LIBRARY}
  ${PNG_LIBRARY}
)

add_subdirectory(src)

