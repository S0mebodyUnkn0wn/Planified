#include "widgets.h"
#include <gtk/gtk.h>
//#include <libadwaita-1/adwaita.h>

struct _PlanifiedPreferencesDialog {
    GtkApplicationWindow parent;
    GSettings *settings;
    GtkSwitch     *weather_autolocation_sw;
    GtkSwitch     *weather_show_station_sw;
    GtkSwitch     *weather_symbolic_icons_sw;
    GtkSpinButton *weather_update_rate;

};

G_DEFINE_TYPE (PlanifiedPreferencesDialog, planified_preferences_dialog, GTK_TYPE_APPLICATION_WINDOW)

static void
dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_PREFERENCES_DIALOG_TYPE);

    G_OBJECT_CLASS (planified_preferences_dialog_parent_class)->dispose(gobject);

}

static void
planified_preferences_dialog_init(PlanifiedPreferencesDialog *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
    self->settings = g_settings_new("com.github.somebodyunknown.planified");
    g_settings_bind(self->settings,"weather-autolocation",self->weather_autolocation_sw,"active",G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(self->settings,"weather-symbolic-icons",self->weather_symbolic_icons_sw,"active",G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(self->settings,"weather-show-station",self->weather_show_station_sw,"active",G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(self->settings,"weather-update-rate",self->weather_update_rate,"value",G_SETTINGS_BIND_DEFAULT);

}

static void
planified_preferences_dialog_class_init(PlanifiedPreferencesDialogClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/planified/preferences-dialog.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),PlanifiedPreferencesDialog,weather_autolocation_sw);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),PlanifiedPreferencesDialog,weather_show_station_sw);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),PlanifiedPreferencesDialog,weather_symbolic_icons_sw);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),PlanifiedPreferencesDialog,weather_update_rate);

    GObjectClass *object_class = G_OBJECT_CLASS(class);
    object_class->dispose = dispose;
}

GtkWidget *
planified_preferences_dialog_new(GtkWindow *win, GtkApplication *app) {
    return g_object_new(PLANIFIED_PREFERENCES_DIALOG_TYPE, "transient-for", win, "application", app, NULL);
}
