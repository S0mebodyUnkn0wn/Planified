#include "widgets.h"
#include "common.h"
#include "common-widgets.h"
#include <gtk/gtk.h>


struct _PlanifiedTaskList {
    GtkBox parent;

    GtkWidget *list;
    GtkWidget *task_list_win;
    GtkWidget *filter_tags_button;
    GtkWidget *task_searchbar;
    GListModel *model;
    GListModel *filtered_model;
    GListModel *sorted_model;
};

G_DEFINE_FINAL_TYPE(PlanifiedTaskList, planified_task_list, GTK_TYPE_BOX)


static void
setup_list_item(GtkListItemFactory *factory,
                GtkListItem *list_item) {
    PlanifiedTaskWidget *taskWidget = planified_task_widget_new(NULL);
    gtk_list_item_set_activatable(list_item, FALSE);
    gtk_list_item_set_child(list_item, (GtkWidget *) taskWidget);
}

static void
bind_list_item(GtkListItemFactory *factory,
               GtkListItem *list_item) {

    PlanifiedTask *task = gtk_list_item_get_item(list_item);
    PlanifiedTaskWidget *taskWidget = PLANIFIED_TASK_WIDGET(gtk_list_item_get_child(list_item));
    g_object_set(taskWidget, "task", task, NULL);
}

static void
unbind_list_item(GtkListItemFactory *factory,
                 GtkListItem *list_item) {
    PlanifiedTaskWidget *taskWidget = PLANIFIED_TASK_WIDGET(gtk_list_item_get_child(list_item));
    g_object_set(taskWidget, "task", NULL, NULL);
}

static void
teardown_list_item(GtkListItemFactory *factory,
                   GtkListItem *list_item) {
    gtk_list_item_set_child(list_item, NULL);
}

static gboolean
filter_func(GObject *item,
            gpointer user_data) {
    return TRUE;
}

static gint
sorter_func(gconstpointer a,
            gconstpointer b,
            gpointer user_data) {
    return 0;
}

void
planified_task_list_override_sorter(PlanifiedTaskList *self, GtkSorter *new_sorter) {
    gtk_sort_list_model_set_sorter(GTK_SORT_LIST_MODEL(self->sorted_model), new_sorter);
}

void
planified_task_list_override_filter(PlanifiedTaskList *self, GtkFilter *new_filter) {
    gtk_filter_list_model_set_filter(GTK_FILTER_LIST_MODEL(self->filtered_model), new_filter);
}

const GtkListView *
planified_task_list_get_list_view(PlanifiedTaskList *self){
    return GTK_LIST_VIEW(self->list);
}

GListModel *
planified_task_list_get_model(PlanifiedTaskList *self){
    return G_LIST_MODEL(self->model);
}

void
planified_task_list_setup(PlanifiedTaskList *self) {
    GListModel *raw_model = (GListModel *) planified_app_get_tasks(get_planified_app((GtkWidget *) self));

    GtkFilter *filter = (GtkFilter *) gtk_custom_filter_new((GtkCustomFilterFunc) filter_func, NULL, NULL);
    GListModel *filtered_model = (GListModel *) gtk_filter_list_model_new(raw_model, filter);

    GtkSorter *sorter = (GtkSorter *) gtk_custom_sorter_new((GCompareDataFunc) sorter_func, NULL, NULL);
    GListModel *sorted_model = (GListModel *) gtk_sort_list_model_new(filtered_model, sorter);

    self->filtered_model = filtered_model;
    self->sorted_model = sorted_model;
    self->model = (GListModel *) gtk_single_selection_new(sorted_model);
    gtk_single_selection_set_can_unselect((GtkSingleSelection *) self->model, TRUE);
    gtk_single_selection_set_autoselect((GtkSingleSelection *) self->model, FALSE);
    gtk_list_view_set_model((GtkListView *) self->list, (GtkSelectionModel *) self->model);

    gtk_scrolled_window_set_vadjustment((GtkScrolledWindow *) self->task_list_win, 0);
}

PlanifiedTask *
planified_task_list_get_selected(PlanifiedTaskList *self) {

    return gtk_single_selection_get_selected_item((GtkSingleSelection *) self->model);
}

static void
planified_task_list_init(PlanifiedTaskList *self) {
    gtk_widget_init_template((GtkWidget *) self);
    GtkListItemFactory *item_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (item_factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect (item_factory, "bind", G_CALLBACK(bind_list_item), NULL);
    g_signal_connect (item_factory, "unbind", G_CALLBACK(unbind_list_item), NULL);
    g_signal_connect (item_factory, "teardown", G_CALLBACK(teardown_list_item), NULL);

    self->list = gtk_list_view_new(NULL, item_factory);
    gtk_scrollable_set_vscroll_policy((GtkScrollable *) self->list, GTK_SCROLL_NATURAL);

    gtk_scrolled_window_set_child((GtkScrolledWindow *) self->task_list_win, self->list);
}


static void
planified_task_list_class_init(PlanifiedTaskListClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/task-list.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTaskList, task_list_win);


}
