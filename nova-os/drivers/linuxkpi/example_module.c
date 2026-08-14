#include "include/nova_linuxkpi.h"

int
main(void)
{
	static const nova_linuxkpi_module_t module = {
		.name = "nova-linuxkpi-example",
		.license = "GPL-2.0-or-later",
		.upstream_revision = "not-vendored"
	};

	return (nova_linuxkpi_register(&module) == 0 &&
	    nova_linuxkpi_module_count() == 1 ? 0 : 1);
}