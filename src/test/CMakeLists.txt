SET(BFILES tget_alpha tget_days)
SET(CFILES tget_alpha.c tget_days.c)

SET_SOURCE_FILES_PROPERTIES(${CFILES} COMPILE_FLAGS "-I..")

ADD_EXECUTABLE(tget_alpha tget_alpha.c)
ADD_EXECUTABLE(tget_days tget_days.c)

SET_TARGET_PROPERTIES(${BFILES} PROPERTIES LINK_FLAGS "-L.. -ltouchstone")

ADD_TEST(alpha sh tget_alpha.sh)
ADD_TEST(days sh tget_days.sh)
