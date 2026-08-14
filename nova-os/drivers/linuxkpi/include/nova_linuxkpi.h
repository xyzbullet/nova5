#ifndef NOVA_LINUXKPI_H
#define NOVA_LINUXKPI_H

#include <stddef.h>

typedef struct {
	const char *name;
	const char *license;
	const char *upstream_revision;
} nova_linuxkpi_module_t;

int nova_linuxkpi_register(const nova_linuxkpi_module_t *module);
size_t nova_linuxkpi_module_count(void);

#endif