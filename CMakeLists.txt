
cmake_minimum_required(VERSION 2.6)

project(Tuk)

find_package(SDL)
find_package(OpenGL)
find_package(PNG)

link_libraries(
  ${SDL_LIBRARY}
  ${OPENGL_LIBRARY}
  ${PNG_LIBRARY}
)

add_subdirectory(src)

