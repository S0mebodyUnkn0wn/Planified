#include <common/types/completable.h>

G_DEFINE_INTERFACE(PlanifiedCompletable, planified_completable, G_TYPE_OBJECT)

static void
planified_completable_default_init(PlanifiedCompletableInterface* iface)
{
	g_object_interface_install_property(
		iface,
		g_param_spec_boolean(
			"is-completed",
			"Is completed",
			"Whether or not the item is competed",
			false,
			G_PARAM_READWRITE
		)
	);
	g_object_interface_install_property(
		iface,
		g_param_spec_boxed(
			"deadline",
			"Deadline",
			"A time by which the item should be completed, can be null",
			G_TYPE_DATE_TIME,
			G_PARAM_READWRITE
		)
	);
	g_object_interface_install_property(
		iface,
		g_param_spec_enum(
			"deadline-precision",
			"Deadline precision",
			"How precise is the deadline property",
			PLANIFIED_TYPE_TIME_PRECISION,
			PLANIFIED_TIME_PRECISION_SECOND,
			G_PARAM_READWRITE)
	);
}

// bool
// planified_completable_is_completed(PlanifiedCompletable* completable)
// {
// 	g_assert(PLANIFIED_IS_COMPLETABLE(completable));
// 	PlanifiedCompletableInterface* iface = PLANIFIED_COMPLETABLE_GET_IFACE(completable);
// 	g_assert(iface->is_completed != NULL);
// 	return iface->is_completed(completable);
// }
//
// void
// planified_completable_set_completed(PlanifiedCompletable* completable, bool completed)
// {
// 	g_assert(PLANIFIED_IS_COMPLETABLE(completable));
// 	PlanifiedCompletableInterface* iface = PLANIFIED_COMPLETABLE_GET_IFACE(completable);
// 	g_assert(iface->set_completed != NULL);
// 	iface->set_completed(completable, completed);
// }
