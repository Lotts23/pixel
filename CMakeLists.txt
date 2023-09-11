cmake_minimum_required(VERSION 3.5)

project(rita)

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

set(SOURCES
    main3.cpp 
    #savefunk.cpp
    #savefunk.h
    #drawfunk.cpp
    #drawfunk.h
    )

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
