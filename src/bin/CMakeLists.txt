ADD_EXECUTABLE(${PROJECT_NAME}-generate-table-data ../generate-table-data.c)

SET_TARGET_PROPERTIES(${PROJECT_NAME}-generate-table-data
		PROPERTIES LINK_FLAGS "-L.. -ltouchstone")

INSTALL(PROGRAMS ${PROJECT_NAME}-generate-table-data
		DESTINATION "${INSTALLDEST}/bin")
