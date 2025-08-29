set(HEADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/headers)
set(SOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/sources)

file(GLOB_RECURSE SOURCES ${SOURCES_DIR}/*.cpp)

include_directories(${HEADERS_DIR})

add_library(core STATIC ${SOURCES})

target_include_directories(core PUBLIC ${HEADERS_DIR})
