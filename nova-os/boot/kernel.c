#include <stddef.h>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef long ssize_t;

#define COM1 0x3F8
#define SERIAL_DATA 0
#define SERIAL_INTERRUPT 1
#define SERIAL_FIFO 2
#define SERIAL_LINE_CONTROL 3
#define SERIAL_MODEM_CONTROL 4
#define SERIAL_LINE_STATUS 5
#define NOVA_UTS_RELEASE "3.1.0-prototype"
#define NOVA_UTS_MACHINE "i386-qemu"
#define NOVA_MAX_TASKS 4

static char command[160];
static size_t command_length;
static unsigned long ticks;

enum nova_syscall {
	NOVA_SYS_write = 1,
	NOVA_SYS_read_console = 2,
	NOVA_SYS_uname = 3,
	NOVA_SYS_getpid = 4,
	NOVA_SYS_uptime = 5,
};

struct nova_utsname {
	const char *sysname;
	const char *nodename;
	const char *release;
	const char *version;
	const char *machine;
};

struct nova_task {
	int pid;
	const char *name;
	const char *state;
};

static const struct nova_task tasks[NOVA_MAX_TASKS] = {
	{0, "kernel", "running"},
	{1, "init", "ready"},
	{2, "novakitd", "ready"},
	{3, "shell", "running"},
};

static inline void
outb(unsigned short port, uint8_t value)
{
	__asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t
inb(unsigned short port)
{
	uint8_t value;
	__asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

static void
serial_init(void)
{
	outb(COM1 + SERIAL_INTERRUPT, 0x00);
	outb(COM1 + SERIAL_LINE_CONTROL, 0x80);
	outb(COM1 + SERIAL_DATA, 0x03);
	outb(COM1 + SERIAL_INTERRUPT, 0x00);
	outb(COM1 + SERIAL_LINE_CONTROL, 0x03);
	outb(COM1 + SERIAL_FIFO, 0xC7);
	outb(COM1 + SERIAL_MODEM_CONTROL, 0x0B);
}

static void
serial_putc(char value)
{
	while ((inb(COM1 + SERIAL_LINE_STATUS) & 0x20) == 0)
		;
	outb(COM1 + SERIAL_DATA, (uint8_t)value);
	if (value == '\n')
		serial_putc('\r');
}

static size_t
nova_strlen(const char *text)
{
	size_t length = 0;
	while (text[length] != '\0')
		length++;
	return length;
}

static void
write_unsigned(unsigned long value)
{
	char digits[20];
	size_t length = 0;
	if (value == 0) {
		serial_putc('0');
		return;
	}
	while (value != 0 && length < sizeof(digits)) {
		digits[length++] = (char)('0' + (value % 10));
		value /= 10;
	}
	while (length != 0)
		serial_putc(digits[--length]);
}

static int
starts_with(const char *text, const char *prefix)
{
	size_t index = 0;
	while (prefix[index] != '\0') {
		if (text[index] != prefix[index])
			return 0;
		index++;
	}
	return 1;
}

static long
nova_syscall_dispatch(enum nova_syscall number, unsigned long arg0,
    unsigned long arg1, unsigned long arg2)
{
	(void)arg2;
	if (number == NOVA_SYS_write) {
		const char *buffer = (const char *)arg0;
		size_t length = (size_t)arg1;
		for (size_t i = 0; i < length; i++)
			serial_putc(buffer[i]);
		return (long)length;
	}
	if (number == NOVA_SYS_uname) {
		struct nova_utsname *uts = (struct nova_utsname *)arg0;
		uts->sysname = "NovaOS";
		uts->nodename = "qemu0";
		uts->release = NOVA_UTS_RELEASE;
		uts->version = "Rapid Kernel Phase 3.1";
		uts->machine = NOVA_UTS_MACHINE;
		return 0;
	}
	if (number == NOVA_SYS_getpid)
		return 3;
	if (number == NOVA_SYS_uptime)
		return (long)ticks;
	return -1;
}

static ssize_t
sys_write(const char *text)
{
	return nova_syscall_dispatch(NOVA_SYS_write, (unsigned long)text,
	    nova_strlen(text), 0);
}

static void
serial_prompt(void)
{
	sys_write("nova> ");
}

static int
command_is(const char *expected)
{
	size_t index = 0;
	while (expected[index] != '\0' && command[index] == expected[index])
		index++;
	return expected[index] == '\0' && command[index] == '\0';
}

static void
print_uname(void)
{
	struct nova_utsname uts;
	(void)nova_syscall_dispatch(NOVA_SYS_uname, (unsigned long)&uts, 0, 0);
	sys_write(uts.sysname); sys_write(" "); sys_write(uts.nodename);
	sys_write(" "); sys_write(uts.release); sys_write(" ");
	sys_write(uts.version); sys_write(" "); sys_write(uts.machine);
	sys_write("\n");
}

static void
print_ps(void)
{
	sys_write("PID STATE   COMMAND\n");
	for (size_t i = 0; i < NOVA_MAX_TASKS; i++) {
		write_unsigned((unsigned long)tasks[i].pid);
		sys_write("   "); sys_write(tasks[i].state); sys_write(" ");
		sys_write(tasks[i].name); sys_write("\n");
	}
}

static void
run_command(void)
{
	command[command_length] = '\0';
	serial_putc('\n');
	if (command_is("help")) {
		sys_write("commands: help, info, uname, ps, echo, syscalls, devices, posix, clear, uptime\n");
	} else if (command_is("info")) {
		sys_write("NovaOS Phase 3.1 Rapid Kernel prototype\n");
		sys_write("hybrid layers: microkernel core, BSD/POSIX personality, NovaKit drivers\n");
		sys_write("boot: Multiboot kernel with serial command line in QEMU\n");
	} else if (command_is("uname")) {
		print_uname();
	} else if (command_is("ps")) {
		print_ps();
	} else if (starts_with(command, "echo ")) {
		sys_write(command + 5); sys_write("\n");
	} else if (command_is("syscalls")) {
		sys_write("implemented syscall ABI: write, uname, getpid, uptime; planned: open/read/close/fork/exec/wait\n");
	} else if (command_is("devices")) {
		sys_write("novakit registry: console0(com1), cpu0(qemu), bus0(multiboot), blk0(virtio planned)\n");
	} else if (command_is("posix")) {
		sys_write("POSIX status: command names and uname/getpid/write semantics are present; full POSIX requires BSD VFS/process integration.\n");
	} else if (command_is("clear")) {
		sys_write("\033[2J\033[H");
	} else if (command_is("uptime")) {
		sys_write("uptime ticks: "); write_unsigned((unsigned long)nova_syscall_dispatch(NOVA_SYS_uptime, 0, 0, 0)); sys_write("\n");
	} else if (command_length != 0) {
		sys_write("unknown command; use help\n");
	}
	command_length = 0;
	serial_prompt();
}

static void
serial_backspace(void)
{
	if (command_length == 0)
		return;
	command_length--;
	sys_write("\b \b");
}

static void
serial_poll(void)
{
	uint8_t value;
	if ((inb(COM1 + SERIAL_LINE_STATUS) & 0x01) == 0)
		return;
	value = inb(COM1 + SERIAL_DATA);
	if (value == '\r' || value == '\n')
		run_command();
	else if (value == '\b' || value == 0x7F)
		serial_backspace();
	else if (value >= 0x20 && value <= 0x7E && command_length < sizeof(command) - 1) {
		command[command_length++] = (char)value;
		serial_putc((char)value);
	}
}

void
kmain(uint32_t multiboot_magic, uint32_t multiboot_info)
{
	(void)multiboot_magic;
	(void)multiboot_info;
	serial_init();
	sys_write("\nNovaOS Phase 3.1 Rapid Kernel booted.\n");
	sys_write("QEMU command line online; type help for POSIX and NovaKit commands.\n");
	serial_prompt();
	for (;;) {
		ticks++;
		serial_poll();
	}
}
