ADD_EXECUTABLE(${PROJECT_NAME}-generate-table-data ../generate-table-data.c)

SET_TARGET_PROPERTIES(${PROJECT_NAME}-generate-table-data
		PROPERTIES LINK_FLAGS "-L.. -ltouchstone")

install(
    PROGRAMS
    ${CMAKE_BINARY_DIR}/src/bin/${PROJECT_NAME}-generate-table-data
    DESTINATION "bin"
)
