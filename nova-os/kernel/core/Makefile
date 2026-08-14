CC?=cc
CFLAGS?=-std=c11 -Wall -Wextra -Werror
TEST_BIN?=../../build/nova-core-test

CORE_SOURCES=scheduler.c ipc.c vm.c

.PHONY: test clean

test: ${TEST_BIN}
	@${TEST_BIN}
	@printf '%s\n' 'NovaOS microkernel core host tests passed.'

${TEST_BIN}: ${CORE_SOURCES} host_test.c include/nova_core.h
	@mkdir -p ../../build
	@${CC} ${CFLAGS} -Iinclude ${CORE_SOURCES} host_test.c -o ${TEST_BIN}

clean:
	@rm -f ${TEST_BIN}