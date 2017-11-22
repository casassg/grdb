file(GLOB SRC_FILES *.c)
add_executable(grdb ${SRC_FILES})
target_link_libraries(grdb db)