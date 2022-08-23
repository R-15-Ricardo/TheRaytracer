cmake_minimum_required(VERSION 3.17)
project(raytracer)

set(CMAKE_CXX_STANDARD 14)

find_package(PNG REQUIRED)
find_package(Boost)
include_directories(${PNG_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
message(STATUS "Boost ${Boost_LIBRARIES}")

add_link_options(-larmadillo ${PNG_LIBRARIES} ${Boost_LIBRARIES} -pthread)
add_executable(raytracer main.cpp Math/Ray.hpp Entity/Camera.cpp Entity/Camera.hpp Material/Material.cpp Material/Material.hpp Entity/LightSource.hpp Entity/Object.cpp Entity/Object.hpp Util/Epsilon.hpp Util/RayCaster.cpp Util/RayCaster.hpp Renderer/Renderer.cpp Renderer/Renderer.hpp Renderer/Scene.hpp Math/AffineTransform.cpp Math/AffineTransform.hpp Renderer/Scene.cpp)