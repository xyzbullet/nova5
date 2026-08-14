#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/systm.h>

static int
nova_core_modevent(module_t module, int event, void *arg)
{
	(void)module;
	(void)arg;

	switch (event) {
	case MOD_LOAD:
		uprintf("NovaOS core module loaded\n");
		return (0);
	case MOD_UNLOAD:
		uprintf("NovaOS core module unloaded\n");
		return (0);
	default:
		return (EOPNOTSUPP);
	}
}

static moduledata_t nova_core_data = {
	"nova_core",
	nova_core_modevent,
	NULL
};

MODULE_VERSION(nova_core, 1);
MODULE_DEPEND(nova_core, kernel, 1, 1, 1);
DECLARE_MODULE(nova_core, nova_core_data, SI_SUB_DRIVERS, SI_ORDER_ANY);