#ifndef PLANNABLE_H
#define PLANNABLE_H
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PLANIFIED_TYPE_PLANNABLE planified_plannable_get_type()
G_DECLARE_INTERFACE(PlanifiedPlannable, planified_plannable, PLANIFIED, PLANNABLE, GObject)


struct _PlanifiedPlannableInterface
{
	GTypeInterface parent_iface;
	bool (*plan)(PlanifiedPlannable* self,
	             GDateTime* date_time);
	bool (*unplan)(PlanifiedPlannable* self);
};

bool planified_plannable_plan(PlanifiedPlannable* self,GDateTime *date_time);
bool planified_plannable_unplan(PlanifiedPlannable* self);

G_END_DECLS

#endif //PLANNABLE_H
