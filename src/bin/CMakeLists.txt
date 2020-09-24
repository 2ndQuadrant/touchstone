ADD_EXECUTABLE(${PROJECT_NAME}-generate-table-data ../generate-table-data.c)

SET_SOURCE_FILES_PROPERTIES(../generate-table-data.c
    COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/src/pcg -I${CMAKE_SOURCE_DIR}/src/pcg/extras")

SET_TARGET_PROPERTIES(${PROJECT_NAME}-generate-table-data
		PROPERTIES LINK_FLAGS "-L${CMAKE_BINARY_DIR}/src -ltouchstone")

install(
    PROGRAMS
    ${CMAKE_BINARY_DIR}/src/bin/${PROJECT_NAME}-generate-table-data
    DESTINATION "bin"
)
