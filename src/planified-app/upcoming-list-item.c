//
// Created by somebody on 26/07/24.
//
#include <gtk/gtk.h>
#include "widgets.h"

struct _PlanifiedUpcomingListItem {
    GtkBox parent;

    GtkLabel *title;
    GtkLabel *description;
    GtkLabel *detail;
    PlanifiedTask *task;

};

G_DEFINE_TYPE(PlanifiedUpcomingListItem, planified_upcoming_list_item, GTK_TYPE_BOX)

static void
update_label_visibility(GtkLabel *label) {
    gtk_widget_set_visible((GtkWidget *) label, strlen(gtk_label_get_label(label)) != 0);
}

static void set_detail_from_task(GtkLabel *label, PlanifiedTask *task) {
    gchar *type;
    GDateTime *date = planified_task_get_most_relevant_date(task, &type);
    GDateTime *now = g_date_time_new_now_local();
    if (type==NULL){
        gtk_label_set_label(label,"");
    }
    else if (strcmp(type, "schedule") == 0) {
        gchar *str = IS_SAME_DAY(now, date) ? g_date_time_format(date, "Scheduled today %H:%M")
                                            : g_date_time_format(date, "Scheduled %a %H:%M");
        gtk_label_set_label(label, str);
        g_free(str);
    } else if (strcmp(type, "deadline") == 0) {
        gchar *str = IS_SAME_DAY(now, date) ? g_date_time_format(date, "Due today")
                                            : g_date_time_format(date, "Due on %A");
        gtk_label_set_label(label, str);
        g_free(str);
    } else if (strcmp(type, "plan-start") == 0) {
        gtk_label_set_label(label, "Planned");
    }
    g_date_time_unref(now);
}


static void
update_label_text(GObject *_task, GParamSpec *pspec, gpointer _label) {
    GtkLabel *label = GTK_LABEL(_label);
    PlanifiedTask *task = PLANIFIED_TASK(_task);
    if (strcmp(pspec->name, "task-text") == 0)
        gtk_label_set_label(label, planified_task_get_task_text(task));
    else if (strcmp(pspec->name, "description") == 0)
        gtk_label_set_label(label, planified_task_get_description(task));
    else if ((strcmp(pspec->name, "schedule") == 0) ||
             (strcmp(pspec->name, "deadline") == 0) ||
             (strcmp(pspec->name, "plan-start") == 0)) {
        set_detail_from_task(label, task);
    }
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
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedUpcomingListItem, detail);

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
    g_signal_connect(task, "notify::schedule", G_CALLBACK(update_label_text), self->detail);
    g_signal_connect(task, "notify::deadline", G_CALLBACK(update_label_text), self->detail);
    g_signal_connect(task, "notify::plan-start", G_CALLBACK(update_label_text), self->detail);

    gtk_label_set_label(self->title, planified_task_get_task_text(task));
    gtk_label_set_label(self->description, planified_task_get_description(task));
    set_detail_from_task(self->detail, task);
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
