SUBDIR=	common mbbsd util

.include <bsd.subdir.mk>

.ORDER: all-common all-mbbsd
.ORDER: all-common all-util

test:
	bazel test --test_output=all //mbbsd_test:mbbsd_test
