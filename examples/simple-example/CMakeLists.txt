set(HEADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/headers)
set(SOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/sources)

file(GLOB_RECURSE SOURCES ${SOURCES_DIR}/*.cpp)

add_executable(simple-example ${SOURCES})

target_include_directories(simple-example PUBLIC ${HEADERS_DIR})
target_link_libraries(simple-example PUBLIC core)
