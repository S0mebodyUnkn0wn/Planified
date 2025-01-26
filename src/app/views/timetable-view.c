#include <app/views/timetable-view.h>

struct _PlanifiedTimetableView
{
	GtkBox parent;

};

G_DEFINE_FINAL_TYPE(PlanifiedTimetableView, planified_timetable_view, GTK_TYPE_BOX)

static void
planified_timetable_view_init(PlanifiedTimetableView* self)
{
	gtk_widget_init_template(GTK_WIDGET(self));
}

static void
planified_timetable_view_class_init(PlanifiedTimetableViewClass* klass)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass),"/planified/app/timetable-view.ui");
}

PlanifiedTimetableView*
planified_timetable_view_new()
{
	return g_object_new(PLANIFIED_TYPE_TIMETABLE_VIEW,nullptr);
}