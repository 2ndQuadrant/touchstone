SET(BFILES tgenerate_analyze tgenerate_plan tgenerate_query tget_alpha
	tget_days)
SET(CFILES tgenerate_analyze.c tgenerate_plan.c tgenerate_query.c tget_alpha.c
	tget_days.c)

SET_SOURCE_FILES_PROPERTIES(${CFILES} COMPILE_FLAGS "-I..")

ADD_EXECUTABLE(tgenerate_analyze tgenerate_analyze.c)
ADD_EXECUTABLE(tgenerate_plan tgenerate_plan.c)
ADD_EXECUTABLE(tgenerate_query tgenerate_query.c)
ADD_EXECUTABLE(tget_alpha tget_alpha.c)
ADD_EXECUTABLE(tget_days tget_days.c)

SET_TARGET_PROPERTIES(${BFILES} PROPERTIES LINK_FLAGS "-L.. -ltouchstone")

ADD_TEST(alpha sh ${CMAKE_SOURCE_DIR}/src/test/tget_alpha.sh)
ADD_TEST(data_generator sh ${CMAKE_SOURCE_DIR}/src/test/tdata_generator.sh)
ADD_TEST(days sh ${CMAKE_SOURCE_DIR}/src/test/tget_days.sh)
ADD_TEST(generate_analyze sh ${CMAKE_SOURCE_DIR}/src/test/tgenerate_analyze.sh)
ADD_TEST(generate_plan sh ${CMAKE_SOURCE_DIR}/src/test/tgenerate_plan.sh)
ADD_TEST(generate_query sh ${CMAKE_SOURCE_DIR}/src/test/tgenerate_query.sh)
