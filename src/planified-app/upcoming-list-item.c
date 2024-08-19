//
// Created by somebody on 26/07/24.
//
#include <gtk/gtk.h>
#include "widgets.h"

struct _PlanifiedUpcomingListItem {
    GtkBox parent;

    GtkLabel *title;
    GtkLabel *description;
    PlanifiedTask *task;

};

G_DEFINE_TYPE(PlanifiedUpcomingListItem, planified_upcoming_list_item, GTK_TYPE_BOX)

static void
update_label_visibility(GtkLabel *label) {
    gtk_widget_set_visible((GtkWidget *) label, strlen(gtk_label_get_label(label)) != 0);
}

static void
update_label_text(GObject *_task, GParamSpec *pspec, gpointer _label) {
    GtkLabel *label = GTK_LABEL(_label);
    PlanifiedTask *task = PLANIFIED_TASK(_task);
    if (strcmp(pspec->name, "task-text") == 0)
        gtk_label_set_label(label, planified_task_get_task_text(task));
    if (strcmp(pspec->name, "description") == 0)
        gtk_label_set_label(label, planified_task_get_description(task));
}

static void
dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), PLANIFIED_UPCOMING_LIST_ITEM_TYPE);

    G_OBJECT_CLASS(planified_upcoming_list_item_parent_class)->dispose(gobject);
}

static void
planified_upcoming_list_item_init(PlanifiedUpcomingListItem *self) {
    gtk_widget_init_template(GTK_WIDGET(self));

    g_signal_connect_swapped(self->description, "notify::label", G_CALLBACK(update_label_visibility),
                             self->description);
}

static void
planified_upcoming_list_item_class_init(PlanifiedUpcomingListItemClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/upcoming-list-item.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedUpcomingListItem, title);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedUpcomingListItem, description);

    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->dispose = dispose;

}

GtkLabel *
planified_upcoming_list_item_get_title(PlanifiedUpcomingListItem *self) {
    return self->title;
}

GtkLabel *
planified_upcoming_list_item_get_description(PlanifiedUpcomingListItem *self) {
    return self->description;
}


void
planified_upcoming_list_item_bind(PlanifiedUpcomingListItem *self, PlanifiedTask *task) {
    g_assert(PLANIFIED_IS_TASK(task));
    g_assert(PLANIFIED_IS_UPCOMING_LIST_ITEM(self));

    g_signal_connect(task, "notify::description", G_CALLBACK(update_label_text), self->description);

    gtk_label_set_label(self->title, planified_task_get_task_text(task));
    gtk_label_set_label(self->description, planified_task_get_description(task));
    update_label_visibility(self->description);
    self->task = task;

}

void
planified_upcoming_list_item_unbind(PlanifiedUpcomingListItem *self) {
    g_assert(PLANIFIED_IS_UPCOMING_LIST_ITEM(self));

    g_signal_handlers_disconnect_by_data(self->task, self->description);
    self->task = NULL;
}

GtkWidget *
planified_upcoming_list_item_new() {
    return g_object_new(PLANIFIED_UPCOMING_LIST_ITEM_TYPE, NULL);
}
