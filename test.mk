BAZEL?=	bazelisk

pre-bazeltest:
	apt update && \
	apt install -y wget bmake gcc g++ clang ccache libc6-dev libevent-dev pkg-config gnupg libflatbuffers-dev flatbuffers-compiler-dev && \
	wget https://github.com/bazelbuild/bazelisk/releases/download/v1.11.0/bazelisk-linux-amd64 -O /usr/local/bin/bazelisk && \
	bazelisk info

bazeltest:
	echo "CC=$(CC) BAZEL=$(BAZEL)" && export CC=$(CC) && $(BAZEL) test --sandbox_debug --test_output=all --subcommands ...

bazelclean:
	$(BAZEL) clean
