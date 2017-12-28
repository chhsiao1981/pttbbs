SUBDIR=	common mbbsd util

# for tests
.if .defined(GTEST_DIR)
SUBDIR+= tests
.endif

.include <bsd.subdir.mk>

.ORDER: all-common all-mbbsd
.ORDER: all-common all-util

# for tests
.if .defined(GTEST_DIR)
.ORDER: all-mbbsd all-tests
.ORDER: all-util all-tests
.endif
