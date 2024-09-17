#include <gtk/gtk.h>
#include "widgets.h"
#include "common.h"
#include "storage.h"
#include "app.h"
#include "derivable-widgets.h"

/*
 * PlanifiedTaskWidget
 *
 * A class for representing and interacting with a task in Planified
 *
 * Features a checkbox, a delete button and a GTKDragSource, as well as complete set of g_lib props for task properties
 *
 */

struct _PlanifiedTaskWidget {
    PlanifiedTaskContainer parent;

    GtkCheckButton *complete_button;
    GtkLabel *task_label;
    GtkLabel *deadline_label;
    GtkButton *delete_task_button;
    GtkListView *tag_grid;
    GtkScrolledWindow *tag_grid_window;

//    GtkPopoverMenu *context_menu;

//    PlanifiedTask *task;

};

G_DEFINE_TYPE(PlanifiedTaskWidget, planified_task_widget, PLANIFIED_TASK_CONTAINER_TYPE)


static void
setup_list_item(GtkListItemFactory *factory,
                GtkListItem *list_item) {
//    g_print("setup\n");

//    gtk_list_item_set_child(list_item, gtk_button_new_with_label("!"));
    gtk_list_item_set_activatable(list_item, FALSE);
}

static void
bind_list_item(GtkListItemFactory *factory,
               GtkListItem *list_item) {
    GtkWidget *widget;
    PlanifiedTag *tag = gtk_list_item_get_item(list_item);
    g_assert(G_IS_OBJECT(tag));
    widget = /*gtk_label_new(planified_tag_get_name(tag));//*/(GtkWidget *) planified_tag_container_new(tag, FALSE);
//    g_assert(G_IS_OBJECT(widget));
//    gtk_widget_set_vexpand(widget, TRUE);
//    gtk_widget_set_valign(widget, GTK_ALIGN_CENTER);
//    gtk_widget_set_halign(widget, GTK_ALIGN_END);
//
    gtk_list_item_set_child(list_item, widget);
//    g_print("bound\n");
}

static void
unbind_list_item(GtkListItemFactory *factory,
                 GtkListItem *list_item) {
}

static void
teardown_list_item(GtkListItemFactory *factory,
                   GtkListHeader *list_item) {
}

static void
refresh_data(GObject *_task, GParamSpec *pspec, gpointer _self) {
//    g_assert(PLANIFIED_IS_TASK_WIDGET(_self));
    PlanifiedTaskWidget *self = PLANIFIED_TASK_WIDGET(_self);

    PlanifiedTask *task_obj = planified_task_container_get_task((PlanifiedTaskContainer *) self);
    if (!PLANIFIED_IS_TASK(task_obj)) {
        return;
    }

    char *task_text = planified_task_get_task_text(planified_task_container_get_task(PLANIFIED_TASK_CONTAINER(self)));

    gtk_label_set_label(self->task_label, task_text);

    if (planified_task_get_schedule(task_obj) != NULL) {
        gtk_widget_set_visible((GtkWidget *) self->deadline_label, TRUE);
        gchar *label_str = g_date_time_format(planified_task_get_schedule(task_obj), "scheduled: %H:%M %d.%m.%Y");
        gtk_label_set_label(self->deadline_label, label_str);
        g_free(label_str);
    } else if (planified_task_get_deadline(task_obj) != NULL) {
        gtk_widget_set_visible((GtkWidget *) self->deadline_label, TRUE);
        gchar *label_str = g_date_time_format(planified_task_get_deadline(task_obj), "deadline: %d.%m.%Y");
        gtk_label_set_label(self->deadline_label, label_str);
        g_free(label_str);
    } else {
        gtk_label_set_label(self->deadline_label, "");
        gtk_widget_set_visible((GtkWidget *) self->deadline_label, FALSE);
    }

    GListStore *tags = planified_task_get_tags(task_obj);
    if (G_IS_OBJECT(tags) && (g_list_model_get_n_items((GListModel *) tags) > 0)) {
//        gtk_widget_set_visible((GtkWidget *) self->tag_grid_window, TRUE);
        // TODO: grid does not take ownership of the model!
        gtk_list_view_set_model(self->tag_grid, (GtkSelectionModel *) gtk_no_selection_new((GListModel *) tags));
    } else {
//        gtk_widget_set_visible((GtkWidget *) self->tag_grid_window, FALSE);
    }
    gtk_check_button_set_active(self->complete_button, planified_task_get_is_complete(task_obj));


//    g_print("Done refreshing task_obj %s (called for: %s)\n", task_text, pspec != NULL ? pspec->name : "NULL");
}

static void
toggle_task_completeness(GtkCheckButton *button,
                         gpointer user_data) {
    PlanifiedTaskWidget *self = PLANIFIED_TASK_WIDGET(user_data);
    if (gtk_widget_get_ancestor(GTK_WIDGET(self), PLANIFIED_APP_WINDOW_TYPE) != NULL) {
        planified_task_set_is_complete(planified_task_container_get_task((PlanifiedTaskContainer *) self),
                                       gtk_check_button_get_active(button));
        database_update_task(get_handle(GTK_WIDGET(self)),
                             planified_task_container_get_task((PlanifiedTaskContainer *) self), 0);
    }
}


static void
planified_task_widget_init(PlanifiedTaskWidget *widget) {
    gtk_widget_init_template(GTK_WIDGET(widget));
    GtkListItemFactory *item_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (item_factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect (item_factory, "bind", G_CALLBACK(bind_list_item), NULL);
    g_signal_connect (item_factory, "unbind", G_CALLBACK(unbind_list_item), NULL);
    g_signal_connect (item_factory, "teardown", G_CALLBACK(teardown_list_item), NULL);
    gtk_list_view_set_factory(widget->tag_grid, item_factory);
//    gtk_scrollable_set_vscroll_policy((GtkScrollable *) widget->tag_grid, GTK_SCROLL_NATURAL);


    g_signal_connect(widget->complete_button, "toggled", G_CALLBACK(toggle_task_completeness), widget);
    g_signal_connect_swapped(widget->delete_task_button, "clicked", G_CALLBACK(planified_task_container_request_delete),
                             widget);

}


static void
planified_task_widget_dispose(GObject *gobject) {

//    PlanifiedTaskWidget *self = planified_task_widget_get_instance_private(PLANIFIED_TASK_WIDGET(gobject));
//    if (PLANIFIED_IS_TASK(self->task)) {
//        g_signal_handlers_disconnect_by_data(self->task, self);
//        self->task = NULL;
//    }

    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_TASK_WIDGET_TYPE);

    G_OBJECT_CLASS (planified_task_widget_parent_class)->dispose(gobject);
}

static void
self_destruct(PlanifiedTaskContainer *self) {
//    PLANIFIED_TASK_CONTAINER_CLASS(planified_task_widget_parent_class)->self_destruct(self);

//    if (PLANIFIED_IS_TASK_CONTAINER(self)) {
//        GtkWidget *row = gtk_widget_get_ancestor((GtkWidget *) self, GTK_TYPE_LIST_BOX_ROW);
//        GtkListBox *list = (GtkListBox *) gtk_widget_get_ancestor((GtkWidget *) self, GTK_TYPE_LIST_BOX);
////        gtk_list_box_remove(list,row);
//    }
}

static void
planified_task_widget_class_init(PlanifiedTaskWidgetClass *class) {

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
                                                "/planified/task-widget.ui");

    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedTaskWidget, complete_button);
    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedTaskWidget, task_label);
    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedTaskWidget, deadline_label);
    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedTaskWidget, delete_task_button);
    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedTaskWidget, tag_grid);
//    gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS(class), PlanifiedTaskWidget, tag_grid_window);

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->dispose = planified_task_widget_dispose;

    PLANIFIED_TASK_CONTAINER_CLASS(class)->refresh_data = refresh_data;
    PLANIFIED_TASK_CONTAINER_CLASS(class)->self_destruct = self_destruct;

}

PlanifiedTask *
planified_task_widget_get_task(PlanifiedTaskWidget *self) {
    return planified_task_container_get_task((PlanifiedTaskContainer *) self);
}

/*
 * Immediately deletes the task represented by `self` from the task database.
 */
void planified_task_widget_delete_task(PlanifiedTaskWidget *self) {
    planified_task_container_delete_task((PlanifiedTaskContainer *) self);
}

/**
 * Creates new PlanifiedTaskWidget
 *
 * @param task The task that will be represented by the widget.
 *             The caller retains ownership of the data.
 *
 * @note The widget's lifetime is bound to the lifetime of the task it represents
 */
PlanifiedTaskWidget *
planified_task_widget_new(PlanifiedTask *task) {

    PlanifiedTaskWidget *self = g_object_new(PLANIFIED_TASK_WIDGET_TYPE,
                                             "task", task,
                                             NULL);
    return self;
}
