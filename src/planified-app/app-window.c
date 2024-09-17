#include <gtk/gtk.h>
#include "common.h"
#include "app.h"
#include "widgets.h"
#include "dialogs.h"

/* PlanifiedAppWindow - a subclass of GApplicationWindow, based on GTK4's tutorial page */
struct _PlanifiedAppWindow {
    GtkApplicationWindow parent;

    GtkWidget *stack;
    GtkPopoverMenu *context_menu;

    PlanifiedTimetableView *timetable;
    PlanifiedHomeView *home;
    PlanifiedPlanningView *planning;
};

G_DEFINE_TYPE(PlanifiedAppWindow, planified_app_window, GTK_TYPE_APPLICATION_WINDOW)


static void // Handler for the go-to action, jumps to requested window by name
go_to_activated(GSimpleAction* action,
                GVariant* parameter,
                gpointer user_data){
    gchar *string;
    PlanifiedAppWindow *self = PLANIFIED_APP_WINDOW(user_data);
    g_variant_get(parameter,"s",&string);
    g_print("%s",string);
    gtk_stack_set_visible_child_name(GTK_STACK(self->stack),string);
    g_free(string);

}


static GActionEntry win_entries[] = {
        {"go-to", go_to_activated, "s", NULL, NULL},
};

static void
context_menu_popup(GtkGestureClick *self,
                   gint n_press,
                   gdouble x,
                   gdouble y,
                   gpointer user_data) {
    GtkPopover *context_menu = user_data;
    g_assert(GTK_IS_POPOVER(context_menu));
    GdkRectangle rect = {(int) x, (int) y, 0, 0};
    gtk_popover_set_pointing_to(context_menu, &rect);
    gtk_popover_popup(context_menu);
}

static void
planified_app_window_init(PlanifiedAppWindow *window) {
    gtk_widget_init_template(GTK_WIDGET(window));

    GtkWidget *home = GTK_WIDGET(planified_home_view_new());
    GtkWidget *timetable = GTK_WIDGET(planified_timetable_view_new());
    GtkWidget *planning = planified_planning_view_new();
    GtkWidget *tlabel3 = gtk_label_new("Test3");

    g_action_map_add_action_entries(G_ACTION_MAP(window),win_entries, G_N_ELEMENTS(win_entries),window);

    GtkBuilder *builder = gtk_builder_new_from_resource("/planified/context-menus.ui");
    gtk_popover_menu_set_menu_model(window->context_menu,
                                    G_MENU_MODEL(gtk_builder_get_object(builder, "win-context-menu")));
    g_object_unref(builder);

    window->timetable = PLANIFIED_TIMETABLE_VIEW(timetable);
    window->home = PLANIFIED_HOME_VIEW(home);
    window->planning = PLANIFIED_PLANNING_VIEW(planning);

    gtk_stack_add_titled(GTK_STACK(window->stack), home, "Home", "Home");
    gtk_stack_add_titled(GTK_STACK(window->stack), timetable, "Timetable", "Timetable");
    gtk_stack_add_titled(GTK_STACK(window->stack), planning, "Planning", "Planning");
    gtk_stack_add_titled(GTK_STACK(window->stack), tlabel3, "Tasks", "Tasks");

    GtkGesture *right_click = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click), GDK_BUTTON_SECONDARY);
    g_signal_connect(right_click, "pressed", G_CALLBACK(context_menu_popup), GTK_POPOVER(window->context_menu));

    gtk_widget_add_controller(GTK_WIDGET(window->stack), GTK_EVENT_CONTROLLER(right_click));

}

static void
planified_app_window_class_init(PlanifiedAppWindowClass *class) {

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
                                                "/planified/app-window.ui");

    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedAppWindow, stack);
    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedAppWindow, context_menu);

}

void
planified_app_window_setup(PlanifiedAppWindow *self) {
    planified_home_view_setup(self->home);
    planified_timetable_view_setup(self->timetable);
    planified_planning_view_setup(self->planning);

}

PlanifiedAppWindow *
planified_app_window_new(PlanifiedApp *app) {
    PlanifiedAppWindow *self = g_object_new(PLANIFIED_APP_WINDOW_TYPE, "application", app, "default-height", 900,
                                            "default-width", 1400, NULL);
//    planified_timetable_view_reload_data(self->timetable); // Populate timetable view with tasks
//    sqlite3_update_hook(planified_app_get_db_handle(app),planified_timetable_schedule_update,self->timetable);
//    g_signal_connect(app,"task-array-updated",(GCallback) planified_timetable_view_update_row,self->timetable);

    return self;
}

void
planified_app_window_open(PlanifiedAppWindow *win,
                          GFile *file) {

}
