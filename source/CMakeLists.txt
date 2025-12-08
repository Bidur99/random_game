cmake_minimum_required(VERSION 3.11)
project(40736653_Project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_STATIC_LIBRARIES TRUE)

add_subdirectory("lib/SFML")
set(SFML_INCS "lib/SFML/include")
link_directories("${CMAKE_BINARY_DIR}/lib/SFML/lib")

add_executable(40736653_Project
    main.cpp
    Block.h
)

target_include_directories(40736653_Project PRIVATE ${SFML_INCS})

target_link_libraries(40736653_Project
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
 
)
