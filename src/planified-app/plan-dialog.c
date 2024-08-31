#include <gtk/gtk.h>
#include "widgets.h"
#include "common-widgets.h"
#include "common.h"
#include "dialogs.h"
#include "app.h"

struct _PlanifiedPlanDialog {
    GtkApplicationWindow parent;


};


G_DEFINE_TYPE(PlanifiedPlanDialog, planified_plan_dialog, GTK_TYPE_APPLICATION_WINDOW)

static void
dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_PLAN_DIALOG_TYPE);

    G_OBJECT_CLASS (planified_plan_dialog_parent_class)->dispose(gobject);
}

static void
planified_plan_dialog_init(PlanifiedPlanDialog *self) {
    gtk_widget_init_template(GTK_WIDGET(self));


}

static void
planified_plan_dialog_class_init(PlanifiedPlanDialogClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/planified/plan-dialog.ui");

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->dispose = dispose;


}


PlanifiedPlanDialog *
planified_plan_dialog_new(PlanifiedAppWindow *win, GtkApplication *app) {
    return g_object_new(PLANIFIED_PLAN_DIALOG_TYPE, "transient-for", win, "application", app, NULL);
}

void
planified_plan_dialog_activated(GSimpleAction *action,
                                GVariant *parameter,
                                gpointer task_widget) {
    PlanifiedPlanDialog *dialog;
    GtkWindow *win;
    GtkApplication *app = (GtkApplication *) get_planified_app(task_widget);
    win = gtk_application_get_active_window(GTK_APPLICATION (app));
    dialog = planified_plan_dialog_new(PLANIFIED_APP_WINDOW(win), GTK_APPLICATION(app));
    gtk_window_present(GTK_WINDOW (dialog));
}
