execute_process(
	COMMAND pandoc -s -t man ${CMAKE_SOURCE_DIR}/man/man3/genrand64_int64.3.md
    OUTPUT_FILE ${CMAKE_BINARY_DIR}/man/man3/genrand64_int64.3
)

install(
    FILES ${CMAKE_BINARY_DIR}/man/man3/genrand64_int64.3
    DESTINATION "share/man/man3"
)
