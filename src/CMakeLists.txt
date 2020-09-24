ADD_LIBRARY(touchstone SHARED dist.c
    pcg/extras/entropy.c
    pcg/pcg-advance-8.c
    pcg/pcg-advance-16.c
    pcg/pcg-advance-32.c
    pcg/pcg-advance-64.c
    pcg/pcg-advance-128.c
    pcg/pcg-global-32.c
    pcg/pcg-global-64.c
    pcg/pcg-output-8.c
    pcg/pcg-output-16.c
    pcg/pcg-output-32.c
    pcg/pcg-output-64.c
    pcg/pcg-output-128.c
    pcg/pcg-rngs-8.c
    pcg/pcg-rngs-16.c
    pcg/pcg-rngs-32.c
    pcg/pcg-rngs-64.c
    pcg/pcg-rngs-128.c
    pcg/pcg-rngs-128.c
    touchstone.c)
SET_SOURCE_FILES_PROPERTIES(dist.c touchstone.c
    COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/src/pcg -I${CMAKE_SOURCE_DIR}/src/pcg/extras")
ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(test)
INSTALL(
    FILES
    ${CMAKE_SOURCE_DIR}/src/touchstone.h
    DESTINATION "include"
)
INSTALL(
    FILES
    ${CMAKE_SOURCE_DIR}/etc/ts-collectd-sy.conf
    DESTINATION "etc/touchstone"
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
