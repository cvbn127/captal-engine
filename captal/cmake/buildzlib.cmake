
file(COPY cmake/zlib/CMakeLists.txt DESTINATION external/zlib)

add_subdirectory(external/zlib EXCLUDE_FROM_ALL)