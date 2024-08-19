#include <gtk/gtk.h>
#include "common.h"
#include "app.h"
#include "widgets.h"
#include "dialogs.h"

/* PlanifiedAppWindow - a subclass of GApplicationWindow, based on GTK4's tutorial page */
struct _PlanifiedAppWindow {
    GtkApplicationWindow parent;

    GtkWidget *stack;
    PlanifiedTimetableView *timetable;
    PlanifiedHomeView *home;
    PlanifiedPlanningView *planning;
};

G_DEFINE_TYPE(PlanifiedAppWindow, planified_app_window, GTK_TYPE_APPLICATION_WINDOW)



//static void
//planified_itinerary_widget_refresh_data(GFileMonitor *self,
//             GFile *file,
//             GFile *other_file,
//             GFileMonitorEvent event_type,
//             gpointer user_data) {
//    g_print("Refresh started (in window)\n");
//
//    PlanifiedAppWindow *window = PLANIFIED_APP_WINDOW(user_data);
//    g_assert(PLANIFIED_IS_APP_WINDOW(window));
//
//    planified_timetable_view_reload_data(window->timetable);
//}


static void
planified_app_window_init(PlanifiedAppWindow *window) {
    gtk_widget_init_template(GTK_WIDGET(window));


    GtkWidget *home = GTK_WIDGET(planified_home_view_new());
    GtkWidget *timetable = GTK_WIDGET(planified_timetable_view_new());
    GtkWidget *planning = planified_planning_view_new();
    GtkWidget *tlabel3 = gtk_label_new("Test3");

    window->timetable = PLANIFIED_TIMETABLE_VIEW(timetable);
    window->home = PLANIFIED_HOME_VIEW(home);
    window->planning = PLANIFIED_PLANNING_VIEW(planning);

    gtk_stack_add_titled(GTK_STACK(window->stack), home, "Home", "Home");
    gtk_stack_add_titled(GTK_STACK(window->stack), timetable, "Timetable", "Timetable");
    gtk_stack_add_titled(GTK_STACK(window->stack), planning, "Planning", "Planning");
    gtk_stack_add_titled(GTK_STACK(window->stack), tlabel3, "Tasks", "Tasks");

}

static void
planified_app_window_class_init(PlanifiedAppWindowClass *class) {

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
                                                "/planified/app-window.ui");

    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedAppWindow, stack);

}

void
planified_app_window_setup(PlanifiedAppWindow *self){
    planified_home_view_setup(self->home);
    planified_timetable_view_setup(self->timetable);
    planified_planning_view_setup(self->planning);

}

PlanifiedAppWindow *
planified_app_window_new(PlanifiedApp *app) {
    PlanifiedAppWindow *self = g_object_new(PLANIFIED_APP_WINDOW_TYPE, "application", app, "default-height", 900, "default-width",1400 ,NULL);
//    planified_timetable_view_reload_data(self->timetable); // Populate timetable view with tasks
//    sqlite3_update_hook(planified_app_get_db_handle(app),planified_timetable_schedule_update,self->timetable);
//    g_signal_connect(app,"task-array-updated",(GCallback) planified_timetable_view_update_row,self->timetable);

    return self;
}

void
planified_app_window_open(PlanifiedAppWindow *win,
                          GFile *file) {

}
