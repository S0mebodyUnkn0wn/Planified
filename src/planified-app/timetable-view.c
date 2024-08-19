#include <gtk/gtk.h>
#include <time.h>
#include "widgets.h"
#include "common.h"
#include "app.h"

struct _PlanifiedTimetableView {
    GtkWidget parent;

    GtkListBox *timetable_task_list;
    GtkBox *timetable_tasks_header;
    GtkBox *timetable_header;
    GtkLabel *timetable_header_label;

    PlanifiedItineraryWidget *timetable_itinerary;

    GtkButton *itinerary_back;
    GtkButton *itinerary_next;

};

G_DEFINE_TYPE(PlanifiedTimetableView, planified_timetable_view, GTK_TYPE_BOX)

//// Handles DND reordering of tasks
//static gboolean
//on_drop(GtkDropTarget *target,
//        const GValue *value,
//        double x,
//        double y,
//        gpointer data) {
//    GtkListBox *self = data;
//    g_assert(GTK_IS_LIST_BOX(self));
//
//    if (G_VALUE_HOLDS(value, PLANIFIED_TASK_WIDGET_TYPE)) {
//        PlanifiedTaskWidget *task_widget = g_value_get_object(value);
//        GtkWidget *closest_target = gtk_widget_get_ancestor(gtk_widget_pick(GTK_WIDGET(self), x, y, GTK_PICK_DEFAULT),
//                                                            GTK_TYPE_LIST_BOX_ROW);
//
//        int index = -1;
//        if (closest_target != NULL) {
//            index = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(closest_target));
//        }
//        gtk_list_box_move_row(GTK_LIST_BOX(self), GTK_WIDGET(task_widget), index);
//
//    } else return FALSE;
//
//    return TRUE;
//}

static void
refresh_labels(PlanifiedItineraryWidget *widget, gpointer user_data) {
    PlanifiedTimetableView *self = PLANIFIED_TIMETABLE_VIEW(user_data);
    gchar *label_text = g_date_time_format(planified_itinerary_widget_get_selected_date(self->timetable_itinerary),
                                           "Agenda for %d %b %Y");
    gtk_label_set_label(self->timetable_header_label,
                        label_text);
}


static void
planified_timetable_view_init(PlanifiedTimetableView *widget) {
    gtk_widget_init_template(GTK_WIDGET(widget));

    refresh_labels(widget->timetable_itinerary, widget);
//    int left_h;
//    gtk_widget_measure(GTK_WIDGET(widget->timetable_header), GTK_ORIENTATION_VERTICAL, -1, NULL, &left_h, NULL, NULL);
//    int right_h;
//    gtk_widget_measure(GTK_WIDGET(widget->timetable_tasks_header), GTK_ORIENTATION_VERTICAL, -1, NULL, &right_h, NULL,
//                       NULL);
//    int longest = left_h > right_h ? left_h : right_h;
//    gtk_widget_set_size_request(GTK_WIDGET(widget->timetable_header), -1, longest);
//    gtk_widget_set_size_request(GTK_WIDGET(widget->timetable_tasks_header), -1, longest);

    GtkDropTarget *drop_target = gtk_drop_target_new(PLANIFIED_TASK_WIDGET_TYPE, GDK_ACTION_MOVE);
//    g_signal_connect(drop_target, "drop", G_CALLBACK(on_drop), widget->timetable_tasks_list);

    g_signal_connect_swapped(widget->itinerary_back, "clicked", G_CALLBACK(planified_itinerary_widget_go_to_prev_day),
                             widget->timetable_itinerary);
    g_signal_connect_swapped(widget->itinerary_next, "clicked", G_CALLBACK(planified_itinerary_widget_go_to_next_day),
                             widget->timetable_itinerary);
    g_signal_connect(widget->timetable_itinerary, "open-date-changed", G_CALLBACK(refresh_labels), widget);

//    gtk_widget_add_controller(GTK_WIDGET(widget->timetable_tasks_list), GTK_EVENT_CONTROLLER(drop_target));


}

static void
planified_timetable_view_class_init(PlanifiedTimetableViewClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/timetable-view.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, timetable_task_list);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, timetable_header_label);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, timetable_header);
//    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, timetable_tasks_header);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, itinerary_back);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, itinerary_next);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, timetable_itinerary);
//    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTimetableView, timetable_agenda);

}

void
planified_timetable_view_setup(PlanifiedTimetableView *self) {
    planified_task_list_setup((PlanifiedTaskList *) self->timetable_task_list);
    planified_itinerary_widget_setup(self->timetable_itinerary);
}


PlanifiedTimetableView *
planified_timetable_view_new() {
    return g_object_new(PLANIFIED_TIMETABLE_VIEW_TYPE, NULL);
}

