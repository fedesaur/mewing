cmake_minimum_required(VERSION 3.10)
project(ecommerce_server)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(POSTGRESQL REQUIRED libpq)
pkg_check_modules(REDIS REQUIRED hiredis)

include_directories(${POSTGRESQL_INCLUDE_DIRS})
include_directories(${REDIS_INCLUDE_DIRS})
include_directories(src)

add_executable(ecommerce_server src/main.cpp lib/con2db lib/con2redis)
target_link_libraries(ecommerce_server ${POSTGRESQL_LIBRARIES} ${REDIS_LIBRARIES})

