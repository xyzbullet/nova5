PROJECT_ROOT!=		realpath .
BUILD_DIR?=		${PROJECT_ROOT}/build
CONFIG_FILE?=		${PROJECT_ROOT}/config/nova.toml.example

.PHONY: help check configure phase2-test image clean

help:
	@printf '%s\n' \
		'NovaOS development targets:' \
		'  make check      Validate the repository skeleton and required tools' \
		'  make configure  Generate a local build manifest' \
		'  make phase2-test Run the host-testable kernel and NovaKit checks' \
		'  make image      Build a bootable image (requires FreeBSD image tools)' \
		'  make clean      Remove generated build output'

check:
	@sh ./scripts/check-tree.sh

configure: check
	@sh ./scripts/configure.sh "${BUILD_DIR}" "${CONFIG_FILE}"

phase2-test: check
	@${MAKE} -C kernel/core test
	@${MAKE} -C kernel/novakit test
	@${MAKE} -C drivers/linuxkpi test

image: configure
	@sh ./scripts/build-image.sh "${BUILD_DIR}"

clean:
	@rm -rf "${BUILD_DIR}"