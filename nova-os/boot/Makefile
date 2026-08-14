CC?=cc
NASM?=nasm
LD?=ld
GRUB_MKRESCUE?=grub-mkrescue
BUILD_DIR?=../build

CFLAGS?=-m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin \
		-fno-asynchronous-unwind-tables -Wall -Wextra -Werror
LDFLAGS?=-m elf_i386 -nostdlib

KERNEL=${BUILD_DIR}/nova-kernel
IMAGE=${BUILD_DIR}/novaos-prototype.iso

.PHONY: all image clean

all: image

image: ${IMAGE}

${BUILD_DIR}/boot.o: boot.asm
	@mkdir -p ${BUILD_DIR}
	@${NASM} -f elf32 $< -o $@

${BUILD_DIR}/kernel.o: kernel.c
	@mkdir -p ${BUILD_DIR}
	@${CC} ${CFLAGS} -c $< -o $@

${KERNEL}: ${BUILD_DIR}/boot.o ${BUILD_DIR}/kernel.o linker.ld
	@${LD} ${LDFLAGS} -T linker.ld ${BUILD_DIR}/boot.o \
		${BUILD_DIR}/kernel.o -o $@

${IMAGE}: ${KERNEL} grub.cfg
	@mkdir -p ${BUILD_DIR}/iso/boot/grub
	@cp ${KERNEL} ${BUILD_DIR}/iso/boot/nova-kernel
	@cp grub.cfg ${BUILD_DIR}/iso/boot/grub/grub.cfg
	@${GRUB_MKRESCUE} -o $@ ${BUILD_DIR}/iso >/dev/null

clean:
	@rm -rf ${BUILD_DIR}/boot.o ${BUILD_DIR}/kernel.o \
		${KERNEL} ${IMAGE} ${BUILD_DIR}/iso