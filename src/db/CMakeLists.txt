file(GLOB SRC_FILES *.c)
add_library(db ${SRC_FILES} dinamic_array.c)