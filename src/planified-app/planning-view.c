#include "widgets.h"
#include "app.h"
#include "common-widgets.h"
#include "common.h"
#include <gtk/gtk.h>


struct _PlanifiedPlanningView {
    GtkBox parent;

    GtkWidget *planning_task_list;
    GtkWidget *planning_day_plans;
    GtkWidget *planning_calendar;

    GtkWidget *selected_day_list;
    GtkWidget *selected_day_timetable;

    GtkLabel *details_task_title;
    GtkLabel *details_task_description;
    GtkLabel *details_task_planned;
    GtkLabel *details_task_complete;
    GtkLabel *details_task_timereq;
    GtkLabel *details_task_deadline;

    GtkLabel *selected_day_date_label;
};

G_DEFINE_FINAL_TYPE(PlanifiedPlanningView, planified_planning_view, GTK_TYPE_BOX)


static void
on_date_selected(GtkCalendar *calendar,
                 gpointer _self) {
    GDateTime *date = gtk_calendar_get_date(calendar);
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(_self);
    gchar *date_string = g_date_time_format(date, "Your plans for %A, %B %-e");
    gchar *quantifier;
    int dm = g_date_time_get_day_of_month(date);
    switch ((dm % 10) * (dm < 10 || dm >= 14)) {
        case 1:
            quantifier = "st:";
            break;
        case 2:
            quantifier = "nd:";
            break;
        case 3:
            quantifier = "rd:";
            break;
        default:
            quantifier = "th:";
    }
    gtk_label_set_label(self->selected_day_date_label, strcat(date_string, quantifier));
    planified_itinerary_widget_set_selected_date((PlanifiedItineraryWidget *) self->selected_day_timetable,
                                                 gtk_calendar_get_date((GtkCalendar *) self->planning_calendar));


    g_free(date_string);
    g_date_time_unref(date);
}

static void
planified_planning_view_init(PlanifiedPlanningView *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
    g_signal_connect(self->planning_calendar,"day-selected",G_CALLBACK(on_date_selected),self);

}


static void
planified_planning_view_class_init(PlanifiedPlanningViewClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/planning-view.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, planning_task_list);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, details_task_title);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, details_task_description);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, planning_day_plans);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, planning_calendar);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, selected_day_date_label);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, selected_day_timetable);

}

static void
update_details(GtkSelectionModel *model,
               guint position,
               guint n_items,
               gpointer user_data) {
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);
    PlanifiedTask *task = planified_task_list_get_selected((PlanifiedTaskList *) self->planning_task_list);
    gtk_label_set_label(self->details_task_title, planified_task_get_task_text(task));
    if (strlen(planified_task_get_description(task)) > 0) {
        gtk_widget_set_visible((GtkWidget *) self->details_task_description, TRUE);
        gtk_label_set_label(self->details_task_description, planified_task_get_description(task));
    } else {
        gtk_widget_set_visible((GtkWidget *) self->details_task_description, FALSE);
    }
}


void planified_planning_view_setup(PlanifiedPlanningView *self) {
    planified_task_list_setup((PlanifiedTaskList *) self->planning_task_list);
    planified_itinerary_widget_setup(PLANIFIED_ITINERARY_WIDGET(self->selected_day_timetable));

    GListModel *model = (GListModel *) planified_task_list_get_model((PlanifiedTaskList *) self->planning_task_list);
    g_signal_connect(model, "selection-changed", G_CALLBACK(update_details), self);
    update_details(NULL, 0, 0, self);
    on_date_selected((GtkCalendar *) self->planning_calendar, self);

}


GtkWidget *
planified_planning_view_new() {
    return g_object_new(PLANIFIED_PLANNING_VIEW_TYPE, NULL);
}
