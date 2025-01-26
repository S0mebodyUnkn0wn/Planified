#ifndef PLANABLE_H
#define PLANABLE_H
#include <gtk/gtk.h>
#include <common/types/enums.h>

G_BEGIN_DECLS

#define PLANIFIED_TYPE_PLANABLE planified_planable_get_type()
G_DECLARE_INTERFACE(PlanifiedPlanable, planified_planable, PLANIFIED, PLANABLE, GObject)

struct _PlanifiedPlanableInterface
{
	GTypeInterface parent_iface;
	bool (*plan)(PlanifiedPlanable* self,
	             GDateTime* date_time, PlanifiedTimePrecision precision);
	bool (*unplan)(PlanifiedPlanable* self);
};

bool planified_planable_plan(PlanifiedPlanable* self, GDateTime* date_time, PlanifiedTimePrecision precision);
bool planified_planable_unplan(PlanifiedPlanable* self);




G_END_DECLS

#endif //PLANABLE_H
