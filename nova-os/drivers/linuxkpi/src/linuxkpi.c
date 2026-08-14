#include "../include/nova_linuxkpi.h"

#define NOVA_LINUXKPI_MAX_MODULES 32u

static const nova_linuxkpi_module_t *modules[NOVA_LINUXKPI_MAX_MODULES];
static size_t module_count;

int
nova_linuxkpi_register(const nova_linuxkpi_module_t *module)
{
	if (module == NULL || module->name == NULL || module->license == NULL ||
	    module->upstream_revision == NULL ||
	    module_count == NOVA_LINUXKPI_MAX_MODULES)
		return (-1);
	modules[module_count++] = module;
	return (0);
}

size_t
nova_linuxkpi_module_count(void)
{
	return (module_count);
}