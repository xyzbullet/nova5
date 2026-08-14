typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;

#define COM1 0x3F8
#define SERIAL_DATA 0
#define SERIAL_INTERRUPT 1
#define SERIAL_FIFO 2
#define SERIAL_LINE_CONTROL 3
#define SERIAL_MODEM_CONTROL 4
#define SERIAL_LINE_STATUS 5

static char command[80];
static size_t command_length;

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

static void
serial_write(const char *text)
{
	while (*text != '\0')
		serial_putc(*text++);
}

static void
serial_prompt(void)
{
	serial_write("nova> ");
}

static void
serial_backspace(void)
{
	if (command_length == 0)
		return;
	command_length--;
	serial_write("\b \b");
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
run_command(void)
{
	command[command_length] = '\0';
	serial_putc('\n');

	if (command_is("help")) {
		serial_write("commands: help, info, clear, uptime\n");
	} else if (command_is("info")) {
		serial_write("NovaOS Phase 2 prototype\n");
		serial_write("microkernel core: scheduler ipc vm [host-validated]\n");
		serial_write("bsd personality: integration pending\n");
		serial_write("novakit: registry prototype\n");
	} else if (command_is("clear")) {
		serial_write("\033[2J\033[H");
	} else if (command_is("uptime")) {
		serial_write("uptime: running under QEMU\n");
	} else if (command_length != 0) {
		serial_write("unknown command; use help\n");
	}

	command_length = 0;
	serial_prompt();
}

static void
serial_poll(void)
{
	uint8_t value;

	if ((inb(COM1 + SERIAL_LINE_STATUS) & 0x01) == 0)
		return;
	value = inb(COM1 + SERIAL_DATA);
	if (value == '\r' || value == '\n') {
		run_command();
	} else if (value == '\b' || value == 0x7F) {
		serial_backspace();
	} else if (value >= 0x20 && value <= 0x7E &&
	    command_length < sizeof(command) - 1) {
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
	serial_write("\nNovaOS Phase 2 prototype booted.\n");
	serial_write("This is the QEMU shell milestone; BSD integration is next.\n");
	serial_prompt();

	for (;;) {
		serial_poll();
	}
}