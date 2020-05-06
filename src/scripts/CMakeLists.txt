ADD_SUBDIRECTORY(pgsql)
set(SRC_SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/src/scripts)
set(
    SRC_SCRIPTS_PROGRAMS
    ${CMAKE_SOURCE_DIR}/src/scripts/ts-plot-collectd
    ${CMAKE_SOURCE_DIR}/src/scripts/ts-plot-pidstat
    ${CMAKE_SOURCE_DIR}/src/scripts/ts-plot-queries
    ${CMAKE_SOURCE_DIR}/src/scripts/ts-plot-sar
    ${CMAKE_SOURCE_DIR}/src/scripts/ts-sysstat
)
install(PROGRAMS ${SRC_SCRIPTS_PROGRAMS} DESTINATION "bin")
