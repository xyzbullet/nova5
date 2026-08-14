CC?=cc
CFLAGS?=-std=c11 -Wall -Wextra -Werror
TEST_BIN?=../../build/nova-linuxkpi-test

.PHONY: test

test: ${TEST_BIN}
	@${TEST_BIN}
	@printf '%s\n' 'NovaOS LinuxKPI isolation test passed.'

${TEST_BIN}: src/linuxkpi.c example_module.c include/nova_linuxkpi.h
	@mkdir -p ../../build
	@${CC} ${CFLAGS} -Iinclude src/linuxkpi.c example_module.c -o ${TEST_BIN}