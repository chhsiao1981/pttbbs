SUBDIR=	common mbbsd util

.include <bsd.subdir.mk>

.ORDER: all-common all-mbbsd
.ORDER: all-common all-util

test:
	bazelisk test --sandbox_debug --test_output=all ...
