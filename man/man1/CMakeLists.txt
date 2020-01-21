execute_process(
	COMMAND pandoc -s -t man ${CMAKE_SOURCE_DIR}/man/man1/ts-sysstat.1.md
    OUTPUT_FILE ${CMAKE_BINARY_DIR}/man/man1/ts-sysstat.1
)

install(
    FILES ${CMAKE_BINARY_DIR}/man/man1/ts-sysstat.1
    DESTINATION "share/man/man1"
)
