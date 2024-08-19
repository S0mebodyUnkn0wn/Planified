//
// Created by somebody on 03/06/24.
//
#include <libgweather/gweather.h>

#include <gtk/gtk.h>
#include "app.h"
#include "widgets.h"

/* PlanifiedHomeView
 * A class representing the home tab, has a clock, a weather widget (PlanifiedWeatherWidget) and some task information
 */
struct _PlanifiedHomeView {
    GtkBox parent;

    GtkWidget *home_clock;
    GtkWidget *home_date;

    PlanifiedUpcomingList *upcoming;

    guint clock_updater_id; //TODO: Add custom destructor to deinit clock_update ticker
};

G_DEFINE_TYPE(PlanifiedHomeView, planified_home_view, GTK_TYPE_BOX)

/*
 * GTickCallback function responsible for updating the time displayed on the clock.
 */
static gboolean
update_clock(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer data) {
    GtkWidget *clock_widget = PLANIFIED_HOME_VIEW(widget)->home_clock;
    GtkWidget *date_widget = PLANIFIED_HOME_VIEW(widget)->home_date;

    if (GTK_IS_LABEL(clock_widget)) {
        GDateTime *current_time = g_date_time_new_now_local();
        gtk_label_set_label(GTK_LABEL(clock_widget), g_date_time_format(current_time, "%H:%M:%S"));
        gtk_label_set_label(GTK_LABEL(date_widget), g_date_time_format(current_time, "%A, %B %d"));

        g_date_time_unref(current_time);
        return G_SOURCE_CONTINUE;
    }
    return G_SOURCE_REMOVE;

}

void
planified_home_view_setup(PlanifiedHomeView *self){
    planified_upcoming_list_setup(self->upcoming);
}

static void
planified_home_view_init(PlanifiedHomeView *widget) {
    gtk_widget_init_template(GTK_WIDGET(widget));
    widget->clock_updater_id = gtk_widget_add_tick_callback(GTK_WIDGET(widget), update_clock, NULL, NULL);

}

static void
planified_home_view_class_init(PlanifiedHomeViewClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/home-view.ui");
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedHomeView, home_clock);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedHomeView, home_date);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedHomeView, upcoming);


}

PlanifiedHomeView *
planified_home_view_new() {
    PlanifiedHomeView *widget = g_object_new(PLANIFIED_HOME_VIEW_TYPE, NULL);
    return widget;
}
