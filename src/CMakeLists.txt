ADD_LIBRARY(touchstone SHARED dist.c mt19937-64 touchstone.c)
ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(test)
INSTALL(
    FILES
    ${CMAKE_SOURCE_DIR}/src/touchstone.h
    DESTINATION "include"
)
INSTALL(
    PROGRAMS
    ${CMAKE_BINARY_DIR}/src/libtouchstone.so
    DESTINATION "lib"
)
SET_TARGET_PROPERTIES(touchstone PROPERTIES LINK_FLAGS "-lm")

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/src/config.h.in
    ${CMAKE_BINARY_DIR}/src/bin/config.h
)

ADD_SUBDIRECTORY(bin)
