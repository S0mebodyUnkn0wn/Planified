#include "widgets.h"
#include "app.h"
#include "common-widgets.h"
#include "common.h"
#include "dialogs.h"
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
    GtkSorter *section_sorter;
    GListModel *day_model;
};

G_DEFINE_FINAL_TYPE(PlanifiedPlanningView, planified_planning_view, GTK_TYPE_BOX)


static void
on_date_selected(GtkCalendar *calendar,
                 gpointer _self) {
    GDateTime *date = gtk_calendar_get_date(calendar);
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(_self);
    gchar *date_string = g_date_time_format(date, "Your plans for %A, %B %-e");

    gchar *quantifier = get_quantifier(g_date_time_get_day_of_month(date));

    gtk_label_set_label(self->selected_day_date_label, strcat(date_string, quantifier));
    planified_itinerary_widget_set_selected_date((PlanifiedItineraryWidget *) self->selected_day_timetable,
                                                 gtk_calendar_get_date((GtkCalendar *) self->planning_calendar));
    gtk_list_view_set_model((GtkListView *) self->selected_day_list, NULL);
    gtk_list_view_set_model((GtkListView *) self->selected_day_list, (GtkSelectionModel *) self->day_model);
    gtk_filter_changed(self->filter_relevant, GTK_FILTER_CHANGE_DIFFERENT);
    if (g_list_model_get_n_items(G_LIST_MODEL(self->day_model)) != 0)
        gtk_section_model_sections_changed((GtkSectionModel *) self->day_model, 0,
                                           g_list_model_get_n_items(G_LIST_MODEL(self->day_model)));


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
        (plan_start != NULL &&
         g_date_time_compare_fixed(open_date, plan_start, PLANIFIED_COMPARISON_PRECISION_DAY) >= 0 &&
         g_date_time_compare_fixed(open_date, plan_end, PLANIFIED_COMPARISON_PRECISION_DAY) <= 0)) {
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
    GDateTime *sched_a = planified_task_get_schedule(task_a);
    GDateTime *deadline_a = planified_task_get_deadline(task_a);

    GDateTime *sched_b = planified_task_get_schedule(task_b);
    GDateTime *deadline_b = planified_task_get_deadline(task_b);
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);
    GDateTime *selected_date = gtk_calendar_get_date((GtkCalendar *) self->planning_calendar);

    int res = 0;

    bool b_is_sched = sched_b != NULL && IS_SAME_DAY(sched_b, selected_date);
    bool b_is_deadline = deadline_b != NULL && IS_SAME_DAY(deadline_b, selected_date);

    if (sched_a != NULL && IS_SAME_DAY(sched_a, selected_date)) {
        // a is a sched, b is a equal if sched, otherwise lower
        res = b_is_sched ? 0
                         : 1;
    } else if (deadline_a != NULL && IS_SAME_DAY(deadline_a, selected_date)) {
        // a is a deadline, b is higher if sched, otherwise if b is deadline b id equal, otherwise lower
        res = b_is_sched ? 1
                         : b_is_deadline ? 0
                                         : -1;
    } else { // a is a plan, b is higher if sched or deadline
        res = b_is_sched || b_is_deadline ? 1
                                          : 0;
    }


    g_date_time_unref(selected_date);

    return res;
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
    } else if (plan_start != NULL &&
               g_date_time_compare_fixed(selected_date, plan_start, PLANIFIED_COMPARISON_PRECISION_DAY) >= 0 &&
               g_date_time_compare_fixed(selected_date, plan_end, PLANIFIED_COMPARISON_PRECISION_DAY) <= 0) {
        gtk_label_set_label(label, "Tasks in progress:");
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

static gboolean
on_drop(GtkDropTarget *target,
        const GValue *value,
        double x,
        double y,
        gpointer data) {

    PlanifiedPlanningView *self = data;
    g_assert(PLANIFIED_IS_PLANNING_VIEW(self));

    if (G_VALUE_HOLDS(value, PLANIFIED_TASK_TYPE)) {
        PlanifiedTask *task = g_value_get_object(value);
        g_assert(PLANIFIED_IS_TASK(task));

        PlanifiedPlanDialog *dialog = planified_plan_dialog_new(
                (PlanifiedAppWindow *) gtk_widget_get_ancestor((GtkWidget *) self, GTK_TYPE_WINDOW),
                (GtkApplication *) get_planified_app((GtkWidget *) self),
                task);
        GDateTime *date = gtk_calendar_get_date((GtkCalendar *) self->planning_calendar);
        GDateTime *start_date = planified_task_get_plan_start(task);
        if (start_date != NULL && g_date_time_compare_fixed(start_date, date, PLANIFIED_COMPARISON_PRECISION_DAY) < 0)
            planified_plan_dialog_set_end_date(dialog, date);
        else
            planified_plan_dialog_set_start_date(dialog, date);
        g_date_time_unref(date);
        gtk_window_present((GtkWindow *) dialog);
        return TRUE;
    } else return FALSE;
}


static void
planified_planning_view_init(PlanifiedPlanningView *self) {
    gtk_widget_init_template(GTK_WIDGET(self));

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

    GtkDropTarget *drop_target = gtk_drop_target_new(PLANIFIED_TASK_TYPE, GDK_ACTION_MOVE);
    g_signal_connect(drop_target, "drop", G_CALLBACK(on_drop), self);
    g_signal_connect(self->planning_calendar, "day-selected", G_CALLBACK(on_date_selected), self);


    gtk_widget_add_controller(GTK_WIDGET(self->selected_day_list), GTK_EVENT_CONTROLLER(drop_target));


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
update_details(gpointer user_data) {
    PlanifiedPlanningView *self = PLANIFIED_PLANNING_VIEW(user_data);
    PlanifiedTask *task = planified_task_list_get_selected((PlanifiedTaskList *) self->planning_task_list);
    if (task == NULL) {
        gtk_label_set_label(self->details_task_title, "");
        gtk_label_set_label(self->details_task_description, "");
        return;
    }
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
    g_signal_connect_swapped(model, "selection-changed", G_CALLBACK(update_details), self);
    g_signal_connect_swapped(model, "items-changed", G_CALLBACK(update_details), self);

    GtkFilter *filter_relevant = (GtkFilter *) gtk_custom_filter_new((GtkCustomFilterFunc) filter_func, self, NULL);
    self->section_sorter = (GtkSorter *) gtk_custom_sorter_new(sort_to_sections, self, NULL);
    self->filter_relevant = filter_relevant; //FIXED?: list headers do not reload between days
    self->day_model =
            (GListModel *) gtk_sort_list_model_new(
                    (GListModel *) gtk_filter_list_model_new(
                            (GListModel *) planified_app_get_tasks(get_planified_app((GtkWidget *) self)
                            ), filter_relevant),
                    NULL);
    gtk_sort_list_model_set_section_sorter((GtkSortListModel *) self->day_model, self->section_sorter);
    self->day_model = (GListModel *) gtk_no_selection_new(self->day_model);
    gtk_list_view_set_model((GtkListView *) self->selected_day_list,
                            (GtkSelectionModel *) self->day_model);
    update_details(self);
    on_date_selected((GtkCalendar *) self->planning_calendar, self);

//    Glib's datetime api and GtkCalendar are extremely silly, so que crutches:
//    Otherwise calendar keeps track of time and breaks g_date_time_compare().
//    This all could be fixed by compare asking for mask of comparison...
//    GDateTime *now = g_date_time_new_now_local();
//    GDateTime *yesterday = g_date_time_add_days(now, -1);
//    GDateTime *now_zeros = g_date_time_new_local(g_date_time_get_year(now),
//                                                 g_date_time_get_month(now),
//                                                 g_date_time_get_day_of_month(now),
//                                                 0, 0, 0);
//    gtk_calendar_select_day((GtkCalendar *) self->planning_calendar, yesterday);
//    gtk_calendar_select_day((GtkCalendar *) self->planning_calendar, now_zeros);
//    g_date_time_unref(now);
}


GtkWidget *
planified_planning_view_new() {
    return g_object_new(PLANIFIED_PLANNING_VIEW_TYPE, NULL);
}
