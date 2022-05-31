SUBDIR=	common mbbsd util

.include <bsd.subdir.mk>

.ORDER: all-common all-mbbsd
.ORDER: all-common all-util

test:
	bazel test --define 'BBSHOME=\"\\\"$(PWD)/testcase\\\"\"' --sandbox_debug --test_output=all ...
