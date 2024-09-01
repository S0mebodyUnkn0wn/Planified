#include "widgets.h"
#include "app.h"
#include "common-widgets.h"
#include "common.h"
#include <gtk/gtk.h>


struct _PlanifiedPlanningView {
    GtkBox parent;

    GtkWidget *planning_task_list;
    GtkWidget *planning_day_plans;
    GtkWidget *planning_calendar;

    GtkWidget *selected_day_list;
    GtkWidget *selected_day_timetable;

    GtkLabel *details_task_title;
    GtkLabel *details_task_description;
    GtkLabel *details_task_planned;
    GtkLabel *details_task_complete;
    GtkLabel *details_task_timereq;
    GtkLabel *details_task_deadline;

    GtkLabel *selected_day_date_label;
    GtkFilter *filter_relevant;
};

G_DEFINE_FINAL_TYPE(PlanifiedPlanningView, planified_planning_view, GTK_TYPE_BOX)


static void
on_date_selected(GtkCalendar *calendar,
                 gpointer _self) {
    GDateTime *date = gtk_calendar_get_date(calendar);
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(_self);
    gchar *date_string = g_date_time_format(date, "Your plans for %A, %B %-e");

    gchar* quantifier = get_quantifier(g_date_time_get_day_of_month(date));

    gtk_label_set_label(self->selected_day_date_label, strcat(date_string, quantifier));
    planified_itinerary_widget_set_selected_date((PlanifiedItineraryWidget *) self->selected_day_timetable,
                                                 gtk_calendar_get_date((GtkCalendar *) self->planning_calendar));
    gtk_filter_changed(self->filter_relevant, GTK_FILTER_CHANGE_DIFFERENT);

    g_free(date_string);
    g_date_time_unref(date);
}

static gboolean
filter_func(GObject *item,
            gpointer user_data) {

    PlanifiedTask *task = PLANIFIED_TASK(item);
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);

    GDateTime *open_date = gtk_calendar_get_date((GtkCalendar *) self->planning_calendar);
    GDateTime *sched = planified_task_get_schedule(task);
    GDateTime *deadline = planified_task_get_deadline(task);
    GDateTime *plan_start = planified_task_get_plan_start(task);
    GDateTime *plan_end =
            plan_start != NULL ? g_date_time_add_days(plan_start, planified_task_get_plan_span(task)) : NULL;

    bool include = false;

    if ((sched != NULL && IS_SAME_DAY(open_date, sched)) ||
        (deadline != NULL && IS_SAME_DAY(open_date, deadline)) ||
        (plan_start != NULL && g_date_time_compare(open_date, plan_start) >= 0 &&
         g_date_time_compare(open_date, plan_end) <= 0)) {
        include = true;
    }
    if (plan_end)
        g_date_time_unref(plan_end);
    g_date_time_unref(open_date);

    return include;
}

static int
sort_to_sections(gconstpointer a,
                 gconstpointer b,
                 gpointer user_data) {
    PlanifiedTask *task_a = PLANIFIED_TASK(a);
    PlanifiedTask *task_b = PLANIFIED_TASK(b);

    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);

    if (planified_task_get_schedule(task_b) != NULL || planified_task_get_schedule(task_a) != NULL) {
        if (planified_task_get_schedule(task_b) != NULL && planified_task_get_schedule(task_a) != NULL)
            return 0;
        else
            return planified_task_get_schedule(task_a) == NULL ? 1 : -1;
    }
    if (planified_task_get_deadline(task_b) != NULL || planified_task_get_deadline(task_a) != NULL) {
        if (planified_task_get_deadline(task_b) != NULL && planified_task_get_deadline(task_a) != NULL)
            return 0;
        else
            return planified_task_get_deadline(task_a) == NULL ? 1 : -1;
    }
    if (planified_task_get_plan_start(task_b) != NULL || planified_task_get_plan_start(task_a) != NULL) {
        if (planified_task_get_plan_start(task_b) != NULL && planified_task_get_plan_start(task_a) != NULL) {
            return 0;
        }
        else
            return planified_task_get_plan_start(task_a) == NULL ? 1 : -1;
    }
    return 0;
}

static void
setup_list_item(GtkListItemFactory *factory,
                GtkListItem *list_item) {
//    GtkWidget *label;
//    label = gtk_label_new("");
//    gtk_widget_add_css_class(label, "large_font");
//    gtk_widget_set_margin_start(label, 10);
//    gtk_widget_set_margin_top(label, 5);
//    gtk_widget_set_margin_bottom(label, 5);
//    gtk_label_set_xalign((GtkLabel *) label, 0);
//    gtk_list_item_set_child(list_item, label);
    gtk_list_item_set_child(list_item, (GtkWidget *) planified_task_widget_new(NULL));
    gtk_list_item_set_activatable(list_item, FALSE);

}

static void
bind_list_item(GtkListItemFactory *factory,
               GtkListItem *list_item) {
//    GtkWidget *label;
    GtkWidget *task_widget;

    PlanifiedTask *task = gtk_list_item_get_item(list_item);

    g_assert(PLANIFIED_IS_TASK(task));

    task_widget = gtk_list_item_get_child(list_item);
    task = gtk_list_item_get_item(list_item);
//    gtk_label_set_label((GtkLabel *) label, planified_task_get_task_text(task));
    g_object_set(task_widget, "task", task, NULL);

}

static void
unbind_list_item(GtkListItemFactory *factory,
                 GtkListItem *list_item) {
//    GtkWidget *label;
//    label = gtk_list_item_get_child(list_item);
//    gtk_label_set_label((GtkLabel *) label, "");
    GtkWidget *task_widget = gtk_list_item_get_child(list_item);
    g_object_set(task_widget, "task", NULL, NULL);
}

static void
teardown_list_item(GtkListItemFactory *factory,
                   GtkListItem *list_item) {
    gtk_list_item_set_child(list_item, NULL);

}


static void
setup_list_header(GtkListItemFactory *factory,
                  GtkListHeader *list_header) {
    GtkWidget *label;

    label = gtk_label_new("");
    gtk_widget_add_css_class(label, "subtitle_text");
    gtk_label_set_xalign((GtkLabel *) label, 0);
    gtk_list_header_set_child(list_header, label);
}

static void
bind_list_header(GtkListItemFactory *factory,
                 GtkListHeader *list_header,
                 gpointer user_data) {
    GtkLabel *label;

    PlanifiedTask *task = gtk_list_header_get_item(list_header);
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);

    g_assert(PLANIFIED_IS_TASK(task));
    g_assert(GTK_IS_LIST_HEADER(list_header));

    label = GTK_LABEL(gtk_list_header_get_child(list_header));
    GDateTime *sched = planified_task_get_schedule(task);
    GDateTime *deadline = planified_task_get_deadline(task);
    GDateTime *plan_start = planified_task_get_plan_start(task);
    GDateTime *plan_end =
            plan_start != NULL ? g_date_time_add_days(plan_start, planified_task_get_plan_span(task)) : NULL;
    GDateTime *selected_date = gtk_calendar_get_date((GtkCalendar *) self->planning_calendar);


    if (sched != NULL && IS_SAME_DAY(sched, selected_date)) {
        gtk_label_set_label(label, "Scheduled:");
    } else if (deadline != NULL && IS_SAME_DAY(deadline, selected_date)) {
        gtk_label_set_label(label, "Tasks Due:");
    } else if (plan_start != NULL && g_date_time_compare(selected_date, plan_start) >= 0 &&
               g_date_time_compare(selected_date, plan_end) <= 0) {
        gtk_label_set_label(label, "Tasks in the works:");
    }

    g_date_time_unref(selected_date);
    if (plan_end)
        g_date_time_unref(plan_end);
}

static void
unbind_list_header(GtkListItemFactory *factory,
                   GtkListHeader *list_header) {
    GtkWidget *label;
    g_assert(GTK_IS_LIST_HEADER(list_header));

    label = gtk_list_header_get_child(list_header);
    gtk_label_set_label((GtkLabel *) label, "");
}

static void
teardown_list_header(GtkListItemFactory *factory,
                     GtkListHeader *list_header) {
}

static void
planified_planning_view_init(PlanifiedPlanningView *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
    g_signal_connect(self->planning_calendar, "day-selected", G_CALLBACK(on_date_selected), self);

    GtkListItemFactory *item_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (item_factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect (item_factory, "bind", G_CALLBACK(bind_list_item), NULL);
    g_signal_connect (item_factory, "unbind", G_CALLBACK(unbind_list_item), NULL);
    g_signal_connect (item_factory, "teardown", G_CALLBACK(teardown_list_item), NULL);
    GtkListItemFactory *header_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (header_factory, "setup", G_CALLBACK(setup_list_header), NULL);
    g_signal_connect (header_factory, "bind", G_CALLBACK(bind_list_header), self);
    g_signal_connect (header_factory, "unbind", G_CALLBACK(unbind_list_header), NULL);
    g_signal_connect (header_factory, "teardown", G_CALLBACK(teardown_list_header), NULL);
    gtk_list_view_set_factory((GtkListView *) self->selected_day_list, item_factory);
    gtk_list_view_set_header_factory((GtkListView *) self->selected_day_list, header_factory);

}


static void
planified_planning_view_class_init(PlanifiedPlanningViewClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/planified/planning-view.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, planning_task_list);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, details_task_title);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, details_task_description);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, planning_day_plans);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, planning_calendar);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, selected_day_date_label);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, selected_day_timetable);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), PlanifiedPlanningView, selected_day_list);

}

static void
update_details(GtkSelectionModel *model,
               guint position,
               guint n_items,
               gpointer user_data) {
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);
    PlanifiedTask *task = planified_task_list_get_selected((PlanifiedTaskList *) self->planning_task_list);
    gtk_label_set_label(self->details_task_title, planified_task_get_task_text(task));
    if (strlen(planified_task_get_description(task)) > 0) {
        gtk_widget_set_visible((GtkWidget *) self->details_task_description, TRUE);
        gtk_label_set_label(self->details_task_description, planified_task_get_description(task));
    } else {
        gtk_widget_set_visible((GtkWidget *) self->details_task_description, FALSE);
    }
}


void planified_planning_view_setup(PlanifiedPlanningView *self) {
    planified_task_list_setup((PlanifiedTaskList *) self->planning_task_list);
    planified_itinerary_widget_setup(PLANIFIED_ITINERARY_WIDGET(self->selected_day_timetable));

    GListModel *model = (GListModel *) planified_task_list_get_model((PlanifiedTaskList *) self->planning_task_list);
    g_signal_connect(model, "selection-changed", G_CALLBACK(update_details), self);
    GtkFilter *filter_relevant = (GtkFilter *) gtk_custom_filter_new((GtkCustomFilterFunc) filter_func, self, NULL);
    GtkSorter *section_sorter = (GtkSorter *) gtk_custom_sorter_new(sort_to_sections,self,NULL);
    self->filter_relevant = filter_relevant;
    GListModel *day_model = (GListModel *) gtk_sort_list_model_new((GListModel *) gtk_filter_list_model_new(
            (GListModel *) planified_app_get_tasks(get_planified_app((GtkWidget *) self)), filter_relevant), NULL);
    gtk_sort_list_model_set_section_sorter((GtkSortListModel *) day_model, section_sorter);
    gtk_list_view_set_model((GtkListView *) self->selected_day_list,
                            (GtkSelectionModel *) gtk_no_selection_new(day_model));
    update_details(NULL, 0, 0, self);
    on_date_selected((GtkCalendar *) self->planning_calendar, self);

}


GtkWidget *
planified_planning_view_new() {
    return g_object_new(PLANIFIED_PLANNING_VIEW_TYPE, NULL);
}
