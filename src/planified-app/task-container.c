#include <gtk/gtk.h>
#include "derivable-widgets.h"
#include "common.h"
#include "app.h"
#include "storage.h"
#include "dialogs.h"
#include "common-widgets.h"

typedef enum {
    TASK = 1,
    N_PROPERTIES
} PldTaskContainerProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL,};


typedef struct {
    PlanifiedTask *task;
    GtkWidget *context_menu;
    GSimpleActionGroup *action_group;
} PlanifiedTaskContainerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(PlanifiedTaskContainer, planified_task_container, GTK_TYPE_BOX)


static GdkContentProvider *
planified_task_container_on_drag_prepare(GtkDragSource *source,
                                         double x,
                                         double y,
                                         PlanifiedTaskContainer *self) {
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(self);
    return gdk_content_provider_new_typed(PLANIFIED_TASK_TYPE, priv->task);
}

static void
planified_task_container_on_drag_begin(GtkDragSource *source,
                                       GdkDrag *drag,
                                       PlanifiedTaskContainer *self) {
    GdkPaintable *paintable = gtk_widget_paintable_new(GTK_WIDGET (self));
    gtk_drag_source_set_icon(source, paintable, 0, 0);
    g_object_unref(paintable);
}

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
unschedule(PlanifiedTaskContainer *self) {
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(self);
    PlanifiedTask *task = priv->task;
    planified_task_set_schedule(task, NULL);
    database_update_task(get_handle(GTK_WIDGET(self)), task, 0);
}

static void
unschedule_activated(GSimpleAction *action,
                     GVariant *parameter,
                     gpointer self) {
    CommonConfirmDialog *diag = common_confirm_dialog_new(
            GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_WINDOW)),
            "Cancel", "Unschedule",
            "Are you sure?",
            "Do you want to unschedule the task?");
    common_confirm_dialog_connect_confirm_callback(diag, G_CALLBACK(unschedule), self);
    common_confirm_dialog_set_destructive_action(diag, true);
    gtk_window_present(GTK_WINDOW(diag));
}


// Handler for delete action
static void
delete_activated(GSimpleAction *action,
                 GVariant *parameter,
                 gpointer self) {
    g_assert(PLANIFIED_IS_TASK_CONTAINER(self));
    planified_task_container_request_delete(self);
}

static void
on_request_delete_choose(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    GError *err = NULL;

    int button = gtk_alert_dialog_choose_finish(dialog, res, &err);
    if (err) {
        g_printerr("An error occured!\n");
        g_printerr("Error Message: %s\n", err->message);
        return;
    }
    if (button == 1) {
        planified_task_container_delete_task(user_data);
    }
}

/*
 * Asks for user confirmation to delete the task. If confirmed - deletes the task
 */
void planified_task_container_request_delete(PlanifiedTaskContainer *self) {
    const char *buttons[] = {"Cancel", "Delete", NULL};
    char *detail = g_strdup_printf(
            "Are you sure you want to delete the task <span style=\"normal\" alpha=\"75%\" >\"%s\"</span>?\n\n<span weight=\"bold\">You will not be able to restore it!</span>",
            planified_task_get_task_text(planified_task_container_get_task(self)));
    GtkWindow *win = GTK_WINDOW(gtk_widget_get_ancestor((GtkWidget *) self, GTK_TYPE_WINDOW));
    CommonConfirmDialog *del_alert = common_confirm_dialog_new(win, "Cancel", "Delete", "Are You Sure?", detail);
    common_confirm_dialog_set_destructive_action(del_alert, TRUE);
    common_confirm_dialog_connect_confirm_callback(del_alert, G_CALLBACK(planified_task_container_delete_task), self);

    gtk_window_present((GtkWindow *) del_alert);
}

/*
 * Immediately deletes the task represented by `self` from the task database.
 */
void planified_task_container_delete_task(PlanifiedTaskContainer *self) {
//    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(self);
    sqlite3 *handle = get_handle(GTK_WIDGET(self));
    g_assert(handle != NULL);

    database_delete_task(handle, planified_task_container_get_task(self));
}

/*
 * Opens a NewTaskDiag in edit mode for the task
 */
static void
edit_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer self) {
//    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(self);

    GtkApplication *app = gtk_window_get_application(GTK_WINDOW(gtk_widget_get_ancestor(self, GTK_TYPE_WINDOW)));
    GtkWindow *win = gtk_application_get_active_window(app);
    PlanifiedNewentryDialog *dialog = planified_newentry_dialog_editmode_new(PLANIFIED_APP_WINDOW(win), app,
                                                                             planified_task_container_get_task(self));
    gtk_window_present(GTK_WINDOW(dialog));

}

static GActionEntry task_action_entries[] =
        {
                {"delete",     delete_activated,                NULL, NULL, NULL},
                {"plan",       planified_plan_dialog_activated, NULL, NULL, NULL},
                {"unschedule", unschedule_activated,            NULL, NULL, NULL},
                {"edit",       edit_activated,                  NULL, NULL, NULL},
        };


static void
planified_task_container_init(PlanifiedTaskContainer *self) {
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(self);
    gtk_widget_init_template(GTK_WIDGET(self));
    priv->action_group = g_simple_action_group_new();
    g_action_map_add_action_entries(G_ACTION_MAP(priv->action_group), task_action_entries,
                                    G_N_ELEMENTS(task_action_entries),
                                    self);

    GtkDragSource *dragSource = gtk_drag_source_new();
    GtkGesture *right_click = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click), GDK_BUTTON_SECONDARY);
    GtkBuilder *builder = gtk_builder_new_from_resource("/planified/context-menus.ui");
    GMenu *menu = g_menu_new();
    g_menu_append_section(menu, NULL, G_MENU_MODEL(gtk_builder_get_object(builder, "task-context-menu")));
    g_menu_append_section(menu, NULL, G_MENU_MODEL(gtk_builder_get_object(builder, "win-context-menu")));
    gtk_popover_menu_set_menu_model((GtkPopoverMenu *) priv->context_menu,
                                    G_MENU_MODEL(menu));
    g_object_unref(builder);

    g_signal_connect(right_click, "pressed", G_CALLBACK(context_menu_popup), GTK_POPOVER(priv->context_menu));
    g_signal_connect(dragSource, "prepare", G_CALLBACK(planified_task_container_on_drag_prepare), self);
    g_signal_connect(dragSource, "drag-begin", G_CALLBACK(planified_task_container_on_drag_begin), self);

    gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(dragSource));
    gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(right_click));
    gtk_widget_insert_action_group(GTK_WIDGET(self), "task", G_ACTION_GROUP(priv->action_group));

}

static void
self_destruct(PlanifiedTaskContainer *self);

static void
planified_task_container_self_destruct(PlanifiedTaskContainer *self) {
    PLANIFIED_TASK_CONTAINER_GET_CLASS(self)->self_destruct(self);
}

void self_destruct(PlanifiedTaskContainer *self) {
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(self);
    priv->task = NULL;
}


static void refresh_data(GObject *_task, GParamSpec *pspec, gpointer _self) {
    g_assert(PLANIFIED_IS_TASK_CONTAINER(_self));
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(_self);
    g_simple_action_set_enabled( // Disable "unschedule" action if task is not scheduled
            (GSimpleAction *) g_action_map_lookup_action((GActionMap *) priv->action_group, "unschedule"),
            planified_task_get_schedule(priv->task) != NULL);
    PLANIFIED_TASK_CONTAINER_GET_CLASS(_self)->refresh_data(_task, pspec, _self);
}

static void
planified_task_container_set_property(GObject *object,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *pspec) {
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(
            PLANIFIED_TASK_CONTAINER(object));


    switch ((PldTaskContainerProperty) property_id) {
        case TASK:
            if (PLANIFIED_IS_TASK(priv->task)) {
                g_signal_handlers_disconnect_by_data(priv->task, object);
            }
            priv->task = g_value_get_object(value);
            if (priv->task != NULL) {
                g_signal_connect_swapped(priv->task, "task-deleted", G_CALLBACK(planified_task_container_self_destruct),
                                         object);
                g_signal_connect(priv->task, "notify", G_CALLBACK(refresh_data), object);

                refresh_data((GObject *) priv->task, NULL, object);
            }
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            return;
    }

}

static void
planified_task_container_get_property(GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec) {
    PlanifiedTaskContainerPrivate *self = planified_task_container_get_instance_private(
            PLANIFIED_TASK_CONTAINER(object));

    switch ((PldTaskContainerProperty) property_id) {
        case TASK:
            g_value_take_object(value, self->task);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
planified_task_container_dispose(GObject *gobject) {
    PlanifiedTaskContainerPrivate *self = planified_task_container_get_instance_private(
            PLANIFIED_TASK_CONTAINER(gobject));

    if (PLANIFIED_IS_TASK(self->task)) {
        g_signal_handlers_disconnect_by_data(self->task, gobject);
        self->task = NULL;
    }

    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_TASK_CONTAINER_TYPE);

    G_OBJECT_CLASS (planified_task_container_parent_class)->dispose(gobject);
}

PlanifiedTask *
planified_task_container_get_task(PlanifiedTaskContainer *self) {
    PlanifiedTaskContainerPrivate *priv = planified_task_container_get_instance_private(
            PLANIFIED_TASK_CONTAINER(self));

    return priv->task;
}


static void
planified_task_container_class_init(PlanifiedTaskContainerClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
                                                "/planified/task-container.ui");

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), PlanifiedTaskContainer, context_menu);

    obj_properties[TASK] =
            g_param_spec_object("task",
                                "Task",
                                "Task object that is represented by this widget",
                                PLANIFIED_TASK_TYPE,
                                G_PARAM_READWRITE);

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->set_property = planified_task_container_set_property;
    object_class->get_property = planified_task_container_get_property;
    object_class->dispose = planified_task_container_dispose;

    class->self_destruct = self_destruct;

    g_object_class_install_properties(G_OBJECT_CLASS(class), N_PROPERTIES, obj_properties);
}


