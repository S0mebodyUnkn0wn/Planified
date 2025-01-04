#include <common/types/completable.h>

G_DEFINE_INTERFACE(PlanifiedCompletable, planified_completable, G_TYPE_OBJECT)

static void
planified_completable_default_init(PlanifiedCompletableInterface* iface)
{
	g_object_interface_install_property(
		iface,
		g_param_spec_boolean(
			"is-complete",
			"Is complete",
			"Whether or not the item is competed",
			false,
			G_PARAM_READWRITE
		)
	);
}
