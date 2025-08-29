cmake_minimum_required(VERSION 4.1.0)

project(component-engine
    VERSION 0.1.0
    DESCRIPTION "A declarative UI component engine for Python, inspired by React but built with a C++ core."
    HOMEPAGE_URL "https://etib-corp.github.io/component-engine/"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (Python COMPONENTS Interpreter Development.Module Development.Embed REQUIRED)

add_subdirectory(core)
add_subdirectory(examples)