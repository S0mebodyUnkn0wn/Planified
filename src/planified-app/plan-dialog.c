#include <gtk/gtk.h>
#include "widgets.h"
#include "common-widgets.h"
#include "common.h"
#include "dialogs.h"
#include "app.h"
#include "storage.h"

struct _PlanifiedPlanDialog {
    GtkApplicationWindow parent;

    GtkButton *confirm_button;
    CommonDateChooser *start_on;
    CommonDateChooser *finish_by;

    PlanifiedTask *task;
};

typedef enum {
    TASK = 1,
    N_PROPERTIES
} PlanifiedPlanDialogProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

G_DEFINE_TYPE(PlanifiedPlanDialog, planified_plan_dialog, GTK_TYPE_APPLICATION_WINDOW)

static void
dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_PLAN_DIALOG_TYPE);

    G_OBJECT_CLASS (planified_plan_dialog_parent_class)->dispose(gobject);
}

static void
confirm_plan(gpointer _self) {

    PlanifiedPlanDialog *self = PLANIFIED_PLAN_DIALOG(_self);

    if (common_date_chooser_is_date_chosen(self->start_on)) {
        GDateTime *start_date = common_date_chooser_get_chosen_date(self->start_on);
        GDateTime *finish_date = common_date_chooser_get_chosen_date(self->finish_by); // may be NULL

        int span = finish_date != NULL ? (int) (g_date_time_difference(finish_date, start_date) / 1000000 / 3600 / 24)
                                       : 0; //glib datetime api is awful btw
        if (span >= 0) {
            planified_task_set_plan_start(self->task, start_date); //self->task assumes ownership of start_date
            planified_task_set_plan_span(self->task, span);
            if (finish_date != NULL)
                g_date_time_unref(finish_date);
            sqlite3 *handle = planified_app_get_db_handle(PLANIFIED_APP(gtk_window_get_application(GTK_WINDOW(self))));
            database_update_task(handle, self->task, NULL);
            gtk_window_close(GTK_WINDOW(self));
        }
    }
    else if (planified_task_get_plan_start(self->task)!=NULL){
        planified_task_set_plan_start(self->task,NULL);
        planified_task_set_plan_span(self->task,0);
        sqlite3 *handle = planified_app_get_db_handle(PLANIFIED_APP(gtk_window_get_application(GTK_WINDOW(self))));
        database_update_task(handle, self->task, NULL);
        gtk_window_close(GTK_WINDOW(self));
    }

}

static void
set_property(GObject
             *object,
             guint property_id,
             const GValue *value,
             GParamSpec
             *pspec) {
    PlanifiedPlanDialog *self = PLANIFIED_PLAN_DIALOG(object);

    switch ((PlanifiedPlanDialogProperty) property_id) {
        case TASK:
            g_assert(g_value_get_object(value) != NULL);
            self->task = g_value_get_object(value);
            if (planified_task_get_plan_start(self->task) != NULL) {
                GDateTime *end_date = g_date_time_add_days(planified_task_get_plan_start(self->task),
                                                           planified_task_get_plan_span(self->task));
                gtk_editable_set_text(GTK_EDITABLE(self->start_on),
                                      g_date_time_format(planified_task_get_plan_start(self->task), "%d/%m/%Y"));
                gtk_editable_set_text(GTK_EDITABLE(self->finish_by), g_date_time_format(end_date, "%d/%m/%Y"));
                g_date_time_unref(end_date);
            }

            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        case N_PROPERTIES:
            break;
    }
}

static void
get_property(GObject *object,
             guint property_id,
             GValue *value,
             GParamSpec *pspec) {

    PlanifiedPlanDialog *self = PLANIFIED_PLAN_DIALOG(object);

    switch ((PlanifiedPlanDialogProperty) property_id) {
        break;
        case TASK:
            g_value_set_object(value, self->task);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

static void
planified_plan_dialog_init(PlanifiedPlanDialog *self) {
    gtk_widget_init_template(GTK_WIDGET(self));

    g_signal_connect_swapped(self->confirm_button, "clicked", G_CALLBACK(confirm_plan), self);
}

static void
planified_plan_dialog_class_init(PlanifiedPlanDialogClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/planified/plan-dialog.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanDialog, confirm_button);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanDialog, start_on);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanDialog, finish_by);

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->dispose = dispose;
    object_class->set_property = set_property;
    object_class->get_property = get_property;

    obj_properties[TASK] =
            g_param_spec_object("task",
                                "Task",
                                "The task which data will be planned"
                                "The task should remain valid until the dialog closes as the dialog does not ref the task",
                                PLANIFIED_TASK_TYPE,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

void
planified_plan_dialog_set_start_date(PlanifiedPlanDialog *self, GDateTime *date) {
    char *string = g_date_time_format(date, "%d/%m/%Y");
    gtk_editable_set_text(GTK_EDITABLE(self->start_on), string);
    g_free(string);
}

void
planified_plan_dialog_set_end_date(PlanifiedPlanDialog *self, GDateTime *date){
    char *string = g_date_time_format(date, "%d/%m/%Y");
    gtk_editable_set_text(GTK_EDITABLE(self->finish_by), string);
    g_free(string);
}

PlanifiedPlanDialog *
planified_plan_dialog_new(PlanifiedAppWindow *win, GtkApplication *app, PlanifiedTask *task) {
    return g_object_new(PLANIFIED_PLAN_DIALOG_TYPE, "transient-for", win, "application", app, "task", task, NULL);
}

void
planified_plan_dialog_activated(GSimpleAction *action,
                                GVariant *parameter,
                                gpointer task_widget) {
    PlanifiedPlanDialog *dialog;
    GtkWindow *win;
    GtkApplication *app = (GtkApplication *) get_planified_app(task_widget);
    win = gtk_application_get_active_window(GTK_APPLICATION (app));
    dialog = planified_plan_dialog_new(PLANIFIED_APP_WINDOW(win), GTK_APPLICATION(app),
                                       planified_task_container_get_task(PLANIFIED_TASK_CONTAINER(task_widget)));
    gtk_window_present(GTK_WINDOW (dialog));
}
