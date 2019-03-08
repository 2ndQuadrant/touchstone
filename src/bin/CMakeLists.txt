ADD_EXECUTABLE(${PROJECT_NAME}-generate-table-data ../generate-table-data.c)

INSTALL(PROGRAMS ${PROJECT_NAME}-generate-table-data
		DESTINATION "${INSTALLDEST}/bin")
