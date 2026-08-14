CC?=cc
CFLAGS?=-std=c11 -Wall -Wextra -Werror
TEST_BIN?=../../build/novakit-test

.PHONY: test

test: ${TEST_BIN}
	@${TEST_BIN}
	@printf '%s\n' 'NovaKit registry host tests passed.'

${TEST_BIN}: registry.c host_test.c include/novakit.h
	@mkdir -p ../../build
	@${CC} ${CFLAGS} -Iinclude registry.c host_test.c -o ${TEST_BIN}