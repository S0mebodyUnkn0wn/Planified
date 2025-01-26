#ifndef TIMETABLE_VIEW_H
#define TIMETABLE_VIEW_H
#include <gtk/gtk.h>
#include "common/types/types.h"

G_BEGIN_DECLS

#define PLANIFIED_TYPE_TIMETABLE_VIEW (planified_timetable_view_get_type())
G_DECLARE_FINAL_TYPE(PlanifiedTimetableView, planified_timetable_view, PLANIFIED, TIMETABLE_VIEW, GtkBox)

PlanifiedTimetableView*
planified_timetable_view_new();

G_END_DECLS

#endif //TIMETABLE_VIEW_H
