CC?=cc
CFLAGS?=-std=c11 -Wall -Wextra -Werror
TEST_BIN?=../../build/nova-network-test

NETWORK_SOURCES=network_stack.c loopback.c nic_stub.c

.PHONY: test clean

test: ${TEST_BIN}
	@${TEST_BIN}
	@printf '%s\n' 'NovaOS network stack prototype host tests passed.'

${TEST_BIN}: ${NETWORK_SOURCES} host_test.c network_stack.h
	@mkdir -p ../../build
	@${CC} ${CFLAGS} -I. ${NETWORK_SOURCES} host_test.c -o ${TEST_BIN}

clean:
	@rm -f ${TEST_BIN}
