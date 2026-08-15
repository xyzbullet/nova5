PROJECT_ROOT!=		realpath .
BUILD_DIR?=		${PROJECT_ROOT}/build
CONFIG_FILE?=		${PROJECT_ROOT}/config/nova.toml.example

.PHONY: help check configure phase2-test prototype-image prototype-shell prototype-shell-test compat-linux-fixture compat-windows-fixture compat-fixtures compat-host-smoke image clean

help:
	@printf '%s\n' \
		'NovaOS development targets:' \
		'  make check      Validate the repository skeleton and required tools' \
		'  make configure  Generate a local build manifest' \
		'  make phase2-test Run the host-testable kernel and NovaKit checks' \
		'  make prototype-image Build the QEMU Phase 2 prototype ISO' \
		'  make prototype-shell Boot the prototype into its serial shell' \
		'  make prototype-shell-test Script and verify the QEMU serial shell' \
		'  make compat-linux-fixture Build the static Linux ELF hello fixture' \
		'  make compat-windows-fixture Build the Windows PE hello fixture' \
		'  make compat-fixtures Build Linux ELF and Windows PE hello fixtures' \
		'  make compat-host-smoke Host-smoke the hello fixtures without recording NovaOS pass' \
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

prototype-image: check
	@${MAKE} -C boot image

prototype-shell: prototype-image
	@qemu-system-x86_64 \
		-cdrom "${BUILD_DIR}/novaos-prototype.iso" \
		-boot d \
		-m 128M \
		-serial stdio \
		-display none \
		-monitor none \
		-no-reboot \
		-no-shutdown

prototype-shell-test: prototype-image
	@sh ./tools/qemu/scripted-shell.sh "${BUILD_DIR}/novaos-prototype.iso"

compat-linux-fixture: check
	@sh ./tests/compatibility/build-linux-fixture.sh "${BUILD_DIR}/compat"

compat-windows-fixture: check
	@sh ./tests/compatibility/build-windows-fixture.sh "${BUILD_DIR}/compat"

compat-fixtures: check
	@sh ./tests/compatibility/build-hello-fixtures.sh "${BUILD_DIR}/compat"

compat-host-smoke: compat-fixtures
	@sh ./tests/compatibility/run-host-smoke.sh "${BUILD_DIR}/compat"

image: configure
	@sh ./scripts/build-image.sh "${BUILD_DIR}"

clean:
	@rm -rf "${BUILD_DIR}"