#include <common/types/planable.h>
G_DEFINE_INTERFACE(PlanifiedPlanable, planified_planable, G_TYPE_OBJECT)


static void
planified_planable_default_init(PlanifiedPlanableInterface* iface)
{
	g_object_interface_install_property(
		iface,
		g_param_spec_boxed(
			"planned-time",
			"Planned time",
			"The time of the item is planned for (e.g. a start time of an event, a deadline of a task etc), can be null",
			G_TYPE_DATE_TIME,
			G_PARAM_READWRITE
		)
	);
	g_object_interface_install_property(
		iface,
		g_param_spec_uint64(
			"duration",
			"Duration",
			"The duration of the planned item in seconds, can be 0",
			0,
			UINT64_MAX,
			0,
			G_PARAM_READWRITE
		)
	);
	g_object_interface_install_property(
		iface,
		g_param_spec_enum(
			"planned-time-precision",
			"Planned time precision",
			"How precise is the value of planned-time",
			PLANIFIED_TYPE_TIME_PRECISION,
			PLANIFIED_TIME_PRECISION_SECOND,
			G_PARAM_READWRITE
		)
	);
}

bool
planified_planable_unplan(PlanifiedPlanable* self)
{
	PlanifiedPlanableInterface* iface;
	g_assert(PLANIFIED_IS_PLANABLE(self));
	g_return_val_if_fail(PLANIFIED_IS_PLANABLE (self), false);

	iface = PLANIFIED_PLANABLE_GET_IFACE(self);
	g_return_val_if_fail(iface->unplan != NULL, false);
	return iface->unplan(self);
}
