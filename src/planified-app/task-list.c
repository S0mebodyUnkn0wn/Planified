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

    GtkWidget *filters_menu;
    GtkWidget *filters_button;

    GListModel *model;
    GListModel *filtered_model;
    GListModel *sorted_model;
    GSimpleActionGroup *filter_state;
    GtkWidget* context_menu;
};

G_DEFINE_FINAL_TYPE(PlanifiedTaskList, planified_task_list, GTK_TYPE_BOX)


static void
filter_change(GSimpleAction *action,
              GVariant *value,
              gpointer user_data) {
    const gchar *name = g_action_get_name((GAction *) action);
    PlanifiedTaskList *self = PLANIFIED_TASK_LIST(user_data);
    g_simple_action_set_state(action, value);
    if (g_variant_classify(value) == G_VARIANT_CLASS_BOOLEAN) {
        gtk_filter_changed(gtk_filter_list_model_get_filter(GTK_FILTER_LIST_MODEL(self->filtered_model)),
                           g_variant_get_boolean(value) ? GTK_FILTER_CHANGE_LESS_STRICT
                                                        : GTK_FILTER_CHANGE_MORE_STRICT);
    } else {
        gtk_filter_changed(gtk_filter_list_model_get_filter(GTK_FILTER_LIST_MODEL(self->filtered_model)),
                           GTK_FILTER_CHANGE_DIFFERENT);
    }
}

static GActionEntry task_list_entries[] = {
        {"show-completed",     NULL, NULL, "false", filter_change},
        {"show-not-completed", NULL, NULL, "true",  filter_change},

};

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
    PlanifiedTaskList *self = PLANIFIED_TASK_LIST(user_data);
    PlanifiedTask *task = PLANIFIED_TASK(item);
    if (!g_variant_get_boolean(
            g_action_group_get_action_state(G_ACTION_GROUP(self->filter_state), "show-completed"))
        &&
        planified_task_get_is_complete(task)) {
        return FALSE;
    };
    if (!g_variant_get_boolean(
            g_action_group_get_action_state(G_ACTION_GROUP(self->filter_state), "show-not-completed"))
        &&
        !planified_task_get_is_complete(task)) {
        return FALSE;
    };

    return TRUE;
}

static gint
sorter_func(gconstpointer a,
            gconstpointer b,
            gpointer user_data) {
    return 0;
}

//void
//planified_task_list_override_sorter(PlanifiedTaskList *self, GtkSorter *new_sorter) {
//    gtk_sort_list_model_set_sorter(GTK_SORT_LIST_MODEL(self->sorted_model), new_sorter);
//}
//
//void
//planified_task_list_override_filter(PlanifiedTaskList *self, GtkFilter *new_filter) {
//    gtk_filter_list_model_set_filter(GTK_FILTER_LIST_MODEL(self->filtered_model), new_filter);
//}

const GtkListView *
planified_task_list_get_list_view(PlanifiedTaskList *self) {
    return GTK_LIST_VIEW(self->list);
}

GListModel *
planified_task_list_get_model(PlanifiedTaskList *self) {
    return G_LIST_MODEL(self->model);
}

void
planified_task_list_setup(PlanifiedTaskList *self) {
    GListModel *raw_model = (GListModel *) planified_app_get_tasks(get_planified_app((GtkWidget *) self));

    GtkFilter *filter = (GtkFilter *) gtk_custom_filter_new((GtkCustomFilterFunc) filter_func, self, NULL);
    GListModel *filtered_model = (GListModel *) gtk_filter_list_model_new(raw_model, filter);

    GtkSorter *sorter = (GtkSorter *) gtk_custom_sorter_new((GCompareDataFunc) sorter_func, self, NULL);
    GListModel *sorted_model = (GListModel *) gtk_sort_list_model_new(filtered_model, sorter);

    self->filtered_model = filtered_model;
    self->sorted_model = sorted_model;
    self->model = (GListModel *) gtk_single_selection_new(sorted_model);
    gtk_list_view_set_model((GtkListView *) self->list, (GtkSelectionModel *) self->model);

    gtk_scrolled_window_set_vadjustment((GtkScrolledWindow *) self->task_list_win, 0);
}

PlanifiedTask *
planified_task_list_get_selected(PlanifiedTaskList *self) {

    return gtk_single_selection_get_selected_item((GtkSingleSelection *) self->model);
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
planified_task_list_init(PlanifiedTaskList *self) {
    gtk_widget_init_template((GtkWidget *) self);
    GtkListItemFactory *item_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (item_factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect (item_factory, "bind", G_CALLBACK(bind_list_item), NULL);
    g_signal_connect (item_factory, "unbind", G_CALLBACK(unbind_list_item), NULL);
    g_signal_connect (item_factory, "teardown", G_CALLBACK(teardown_list_item), NULL);

//    g_signal_connect_swapped()

    self->filter_state = g_simple_action_group_new();
    g_action_map_add_action_entries(G_ACTION_MAP(self->filter_state), task_list_entries,
                                    G_N_ELEMENTS(task_list_entries), self);

    gtk_widget_insert_action_group(GTK_WIDGET(self), "tasklist", G_ACTION_GROUP(self->filter_state));

    GtkBuilder *builder = gtk_builder_new_from_resource("/planified/filter-popup-menu.ui");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->filters_button),
                                   G_MENU_MODEL(gtk_builder_get_object(builder, "filter-menu")));
//    g_object_unref(builder);
    gtk_builder_add_from_resource(builder,"/planified/context-menus.ui", nullptr);
    GtkGesture *right_click = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(right_click), GDK_BUTTON_SECONDARY);

    GMenu *menu = g_menu_new();
    g_menu_append_section(menu, NULL, G_MENU_MODEL(gtk_builder_get_object(builder, "task-list-context-menu")));
    g_menu_append_submenu(menu, "Set filter", G_MENU_MODEL(gtk_builder_get_object(builder, "filter-menu")));
    g_menu_append_section(menu, NULL, G_MENU_MODEL(gtk_builder_get_object(builder, "win-context-menu")));
    gtk_popover_menu_set_menu_model((GtkPopoverMenu *) self->context_menu,
                                    G_MENU_MODEL(menu));
    g_object_unref(builder);

    g_signal_connect(right_click, "pressed", G_CALLBACK(context_menu_popup), GTK_POPOVER(self->context_menu));

    self->list = gtk_list_view_new(NULL, item_factory);
    gtk_widget_add_css_class(self->list, "rich-list");
    gtk_scrollable_set_vscroll_policy((GtkScrollable *) self->list, GTK_SCROLL_NATURAL);

    gtk_scrolled_window_set_child((GtkScrolledWindow *) self->task_list_win, self->list);
    gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(right_click));

}


static void
planified_task_list_class_init(PlanifiedTaskListClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/task-list.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTaskList, task_list_win);
//    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTaskList, filters_menu);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTaskList, filters_button);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedTaskList, context_menu);


}
