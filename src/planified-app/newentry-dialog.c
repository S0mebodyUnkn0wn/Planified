#include <gtk/gtk.h>
#include "common.h"
#include "app.h"
#include "widgets.h"
#include "storage.h"
#include <stdlib.h>
#include "common-widgets.h"
#include "dialogs.h"
// PlanifiedNewentryDialog, a dialog window for creating new tasks

typedef enum {
    EDIT_MODE = 1,
    EDIT_TASK,
    N_PROPERTIES
} PlanifiedNewentryDialogProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};

struct _PlanifiedNewentryDialog {
    GtkApplicationWindow parent;

    GtkWidget *task_name;
    GtkWidget *task_description;
    GtkWidget *task_deadline;
    GtkWidget *task_timereq;
    GtkWidget *task_tags;
    GtkWidget *confirm_button;

    gboolean edit_mode;
    gint tag_count;
    GHashTable *tags; //TODO Implement
    GList *removed_tags;
    PlanifiedTask *prefill_task;
};

G_DEFINE_TYPE (PlanifiedNewentryDialog, planified_newentry_dialog, GTK_TYPE_APPLICATION_WINDOW)

static void
planified_newentry_dialog_set_property(GObject
                                       *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec
                                       *pspec) {
    PlanifiedNewentryDialog *self = PLANIFIED_NEWENTRY_DIALOG(object);

    switch ((PlanifiedNewentryDialogProperty) property_id) {
        case EDIT_MODE:
            self->edit_mode = g_value_get_boolean(value);
            break;
        case EDIT_TASK:
            self->prefill_task = g_value_get_object(value);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
planified_newentry_dialog_get_property(GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec) {

    PlanifiedNewentryDialog *self = PLANIFIED_NEWENTRY_DIALOG(object);

    switch ((PlanifiedNewentryDialogProperty) property_id) {
        case EDIT_MODE:
            g_value_set_boolean(value, self->edit_mode);
            break;
        case EDIT_TASK:
            g_value_set_object(value, self->prefill_task);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

/*
 * Completes task creation, using data entered by the user into provided entry buffers
 */
static void
newtask_confirmed(PlanifiedNewentryDialog *self) {
    bool is_valid = true;

    GtkEntryBuffer *task_name_buf = gtk_entry_get_buffer(GTK_ENTRY(self->task_name));
    GtkEntryBuffer *task_deadline_buf = gtk_entry_get_buffer(GTK_ENTRY(self->task_deadline));
    GtkEntryBuffer *task_timereq_buf = gtk_entry_get_buffer(GTK_ENTRY(self->task_timereq));

    if (gtk_entry_buffer_get_length(task_name_buf) <= 0)
        is_valid = false;

    char *task_name_text = (char *) gtk_entry_buffer_get_text(task_name_buf);

    if (gtk_entry_buffer_get_length(task_deadline_buf) > 0) {

    }
    GDateTime *deadline = NULL;
    if (common_date_chooser_is_date_chosen(COMMON_DATE_CHOOSER(self->task_deadline))) {
        GDateTime *chosen_date = common_date_chooser_get_chosen_date(COMMON_DATE_CHOOSER(self->task_deadline));
        if (chosen_date == NULL)
            is_valid = false;
        else
            deadline = chosen_date;
    }

    int task_timereq_val = 0;
    if (gtk_entry_buffer_get_length(task_timereq_buf) > 0) {
        const char *t = gtk_entry_buffer_get_text(task_timereq_buf);
        task_timereq_val = (int) strtol(t, NULL, 10);
    }

    if (!is_valid) {
        return;
    }


    GtkTextBuffer *desc_buf = gtk_text_view_get_buffer((GtkTextView *) self->task_description);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(desc_buf, &start);
    gtk_text_buffer_get_end_iter(desc_buf, &end);
    gchar *description = (gchar *) gtk_text_buffer_get_text(desc_buf, &start, &end, FALSE);

    g_assert(GTK_IS_WINDOW(self));

    GtkApplication *_app = gtk_window_get_application(GTK_WINDOW(self));
    PlanifiedApp *app = PLANIFIED_APP(_app);
    gint64 rowid = self->edit_mode ? planified_task_get_rowid(self->prefill_task) : -1;
    GDateTime *schedule = self->edit_mode ? planified_task_get_schedule(self->prefill_task) : NULL;
    gint64 is_complete = self->edit_mode ? planified_task_get_is_complete(self->prefill_task) : 0;
    gchar *location = self->edit_mode ? planified_task_get_location(self->prefill_task) : "";

    PlanifiedTask *task = planified_task_new(task_name_text,
                                             deadline,
                                             task_timereq_val,
                                             NULL,
                                             location,
                                             schedule,
                                             is_complete,
                                             rowid,
                                             description,
                                             NULL, -1);

    sqlite3 *handle = planified_app_get_db_handle(app);
    g_assert(handle != NULL);
    if (!self->edit_mode) {
        database_insert_task(handle, task, &rowid);
    } else {
        g_print("Updating task (%s)\n", planified_task_get_task_text(task));
        database_update_task(handle, task, NULL);
    }
    planified_task_set_rowid(task, rowid);

    // Set tags for newly created task
    guint l = g_list_length(self->removed_tags);
    g_print("len %d\n",l);
    if (l>0 && self->edit_mode){
        for (guint i =0;i<l;i++){
            database_unlink_tag_from_task(handle, g_list_nth_data(self->removed_tags,i),task);
        }
    }
    for (int i = 0; i < self->tag_count; i++) {
        PlanifiedTagContainer *tag_c = PLANIFIED_TAG_CONTAINER(
                gtk_flow_box_child_get_child(gtk_flow_box_get_child_at_index((GtkFlowBox *) self->task_tags, i)));
        PlanifiedTag *tag = planified_tag_container_get_tag(tag_c);
        database_link_tag_to_task(handle, tag, task);
        g_print("Bound\n");
    }
    g_free(description);
    g_object_unref(task);
    gtk_window_close(GTK_WINDOW(self));
}

static void
remove_tag(PlanifiedTagContainer *tag, PlanifiedNewentryDialog *self) {
    self->removed_tags = g_list_append(self->removed_tags,planified_tag_container_get_tag(tag));
    gtk_flow_box_remove((GtkFlowBox *) self->task_tags, (GtkWidget *) tag);
    self->tag_count--;
}

static void
add_tag(PlanifiedNewentryDialog *self) {
    GtkWidget *tag_c = (GtkWidget *) planified_tag_container_new_entry(TRUE);
    gtk_flow_box_insert((GtkFlowBox *) self->task_tags, tag_c, self->tag_count);
    gtk_widget_set_halign(
            (GtkWidget *) gtk_flow_box_get_child_at_index((GtkFlowBox *) self->task_tags, self->tag_count),
            GTK_ALIGN_START);
    g_signal_connect(tag_c, "delete-tag", G_CALLBACK(remove_tag), self);
    self->tag_count++;
}

static void
planified_newentry_dialog_init(PlanifiedNewentryDialog *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
    g_signal_connect_swapped (self->confirm_button, "clicked",
                              (GCallback) newtask_confirmed, self);

    GtkWidget *add_tag_btn = gtk_button_new_from_icon_name("tag-new");
    gtk_widget_set_halign(add_tag_btn, GTK_ALIGN_START);
    gtk_flow_box_append((GtkFlowBox *) self->task_tags, add_tag_btn);
    self->tag_count = 0;
    g_signal_connect_swapped(add_tag_btn, "clicked", G_CALLBACK(add_tag), self);
//    GtkWidget *editable = (GtkWidget *) planified_tag_container_new_entry(TRUE);
//    gtk_box_append((GtkBox *) self->task_tags,editable);

}

static void
dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_NEWENTRY_DIALOG_TYPE);

    G_OBJECT_CLASS (planified_newentry_dialog_parent_class)->dispose(gobject);
}

static void
planified_newentry_dialog_class_init(PlanifiedNewentryDialogClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/planified/newentry-dialog.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedNewentryDialog, task_name);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedNewentryDialog, task_deadline);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedNewentryDialog, task_timereq);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedNewentryDialog, task_tags);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedNewentryDialog, task_description);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedNewentryDialog, confirm_button);

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->set_property = planified_newentry_dialog_set_property;
    object_class->get_property = planified_newentry_dialog_get_property;
    object_class->dispose = dispose;

    // Potentially remove this prop in favour of having edit-task=NULL mean edit-mode=FALSE
    obj_properties[EDIT_MODE] =
            g_param_spec_boolean("edit-mode",
                                 "Edit mode",
                                 "Run dialog in edit mode (changes data of existing task)",
                                 FALSE,
                                 G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    obj_properties[EDIT_TASK] =
            g_param_spec_object("edit-task",
                                "Edit task",
                                "The task which data will be edited (if edit-mode was set to True)"
                                "The task should remain valid until the dialog closes "
                                "(it'll likely be invalidated right after that)",
                                PLANIFIED_TASK_TYPE,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

/*
 * Creates a new PlanifiedNewentryDialog in edit mode
 * Prefills buffers with data taken from `task`,
 * newtask_confirmed will update `task` instead of adding a new entry
 */
PlanifiedNewentryDialog *
planified_newentry_dialog_editmode_new(PlanifiedAppWindow *win, GtkApplication *app, PlanifiedTask *task) {
    PlanifiedNewentryDialog *self = g_object_new(PLANIFIED_NEWENTRY_DIALOG_TYPE,
                                                 "transient-for", win, "application", app,
                                                 "edit-mode", TRUE, "edit-task", task, NULL);
    g_assert(PLANIFIED_IS_TASK(self->prefill_task));
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(self->task_name)),
                              planified_task_get_task_text(self->prefill_task), -1);

    // Prefill the description
    if (planified_task_get_description(self->prefill_task) != NULL) {
        GtkTextBuffer *buf = gtk_text_view_get_buffer((GtkTextView *) self->task_description);
        gtk_text_buffer_set_text(buf, planified_task_get_description(self->prefill_task), -1);
    }

    // Prefill the deadline
    if (planified_task_get_deadline(self->prefill_task) != NULL) {
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(self->task_deadline)),
                                  g_date_time_format(planified_task_get_deadline(self->prefill_task), "%d/%m/%Y"), -1);
    }

    // Prefill the time requirement
    if (planified_task_get_timereq(self->prefill_task) > 0) {
        char *timereq_str = g_strdup_printf("%d", planified_task_get_timereq(self->prefill_task));
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(self->task_timereq)), timereq_str, -1);
    };

    GListModel *tags = (GListModel *) planified_task_get_tags(self->prefill_task);
    for (guint i = 0; i < g_list_model_get_n_items(tags); i++) {
        PlanifiedTag *tag = g_list_model_get_item(tags, i);
        PlanifiedTagContainer *tag_c = planified_tag_container_new(tag, TRUE);
        gtk_flow_box_insert((GtkFlowBox *) self->task_tags, (GtkWidget *) tag_c, self->tag_count);
        gtk_widget_set_halign(
                (GtkWidget *) gtk_flow_box_get_child_at_index((GtkFlowBox *) self->task_tags, self->tag_count),
                GTK_ALIGN_START);
        g_signal_connect(tag_c, "delete-tag", G_CALLBACK(remove_tag), self);
        self->tag_count++;
//        g_object_unref(tag);
    }
    gtk_window_set_title(GTK_WINDOW(self), "Edit a task");
    gtk_button_set_label(GTK_BUTTON(self->confirm_button), "Confirm Edit");

    return self;
}

PlanifiedNewentryDialog *
planified_newentry_dialog_new(PlanifiedAppWindow *win, GtkApplication *app) {
    return g_object_new(PLANIFIED_NEWENTRY_DIALOG_TYPE, "transient-for", win, "application", app, NULL);
}


void
newtask_activated(GSimpleAction *action,
                  GVariant *parameter,
                  gpointer app) {
    PlanifiedNewentryDialog *dialog;
    GtkWindow *win;

    win = gtk_application_get_active_window(GTK_APPLICATION (app));
    dialog = planified_newentry_dialog_new(PLANIFIED_APP_WINDOW(win), GTK_APPLICATION(app));
    gtk_window_present(GTK_WINDOW (dialog));
}
