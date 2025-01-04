#include <common/types/plannable.h>

G_DEFINE_INTERFACE(PlanifiedPlannable, planified_plannable, G_TYPE_OBJECT)


static void
planified_plannable_default_init(PlanifiedPlannableInterface* iface)
{
	g_object_interface_install_property(
		iface,
		g_param_spec_boxed(
			"start-time",
			"Start time",
			"The start time of the planned item",
			G_TYPE_DATE_TIME,
			G_PARAM_READWRITE
		)
	);
	g_object_interface_install_property(
		iface,
		g_param_spec_uint64(
			"duration",
			"Duration",
			"The duration of the planned item in seconds",
			0,
			INT64_MAX,
			0,
			G_PARAM_READWRITE
		)
	);
}

bool
planified_plannable_unplan(PlanifiedPlannable* self)
{
	PlanifiedPlannableInterface *iface;
	g_assert(PLANIFIED_IS_PLANNABLE(self));
	g_return_val_if_fail (PLANIFIED_IS_PLANNABLE (self), false);

	iface = PLANIFIED_PLANNABLE_GET_IFACE (self);
	g_return_val_if_fail (iface->unplan != NULL, false);
	return iface->unplan(self);
}