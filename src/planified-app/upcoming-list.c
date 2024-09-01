#include <gtk/gtk.h>
#include "app.h"
#include "widgets.h"
#include "storage.h"

//typedef enum {
//    N_SIGNALS
//} PlanifiedUpcomingListSignals;
//
//typedef enum {
//    N_PROPERTIES
//} PlanifiedUpcomingListProperties;
//
//static guint upcoming_signals[N_SIGNALS];


struct _PlanifiedUpcomingList {
    GtkBox parent;

    GtkWidget *upcoming_list;
    GtkWidget *scrolled_win;
    GListModel *model;
};

G_DEFINE_TYPE(PlanifiedUpcomingList, planified_upcoming_list, GTK_TYPE_BOX)

void
planified_upcoming_list_refresh_data(PlanifiedUpcomingList *self) {

}


static void
setup_list_item(GtkListItemFactory *factory,
                GtkListItem *list_item) {
    GtkWidget *box;

    box = planified_upcoming_list_item_new();
    gtk_list_item_set_child(list_item, box);
    gtk_list_item_set_activatable(list_item, FALSE);

}

static void
bind_list_item(GtkListItemFactory *factory,
               GtkListItem *list_item) {
    GtkWidget *box;

    GtkLabel *title;
    GtkLabel *description;
    PlanifiedTask *task = gtk_list_item_get_item(list_item);

    g_assert(PLANIFIED_IS_TASK(task));

    box = gtk_list_item_get_child(list_item);
    task = gtk_list_item_get_item(list_item);
    planified_upcoming_list_item_bind((PlanifiedUpcomingListItem *) box, task);

}

static void
unbind_list_item(GtkListItemFactory *factory,
                 GtkListItem *list_item) {
    GtkWidget *box;

    box = gtk_list_item_get_child(list_item);
    planified_upcoming_list_item_unbind((PlanifiedUpcomingListItem *) box);
}

static void
teardown_list_item(GtkListItemFactory *factory,
                   GtkListHeader *list_item) {
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
                 GtkListHeader *list_header) {
    GtkWidget *label;

    PlanifiedTask *task = gtk_list_header_get_item(list_header);

    g_assert(PLANIFIED_IS_TASK(task));
    g_assert(GTK_IS_LIST_HEADER(list_header));

    label = gtk_list_header_get_child(list_header);
    gchar * date_type;
    GDateTime *sched = planified_task_get_most_relevant_date(task, &date_type);
    GDateTime *now = g_date_time_new_now_local();


    if (sched != NULL) {
        gint diff = g_date_time_get_week_of_year(sched) - g_date_time_get_week_of_year(now);
        gint ddiff = g_date_time_get_day_of_year(sched) - g_date_time_get_day_of_year(now);
        if (IS_SAME_DAY(sched, now))
            gtk_label_set_label((GtkLabel *) label, "Today:");
        else if (ddiff < 0)
            gtk_label_set_label((GtkLabel *) label, "Overdue:");
        else {
            switch (diff) {
                case 0:
                    gtk_label_set_label((GtkLabel *) label, "This Week:");
                    break;
                case 1:
                    gtk_label_set_label((GtkLabel *) label, "Next Week:");
                    break;
                default:
                    gtk_label_set_label((GtkLabel *) label, g_strdup_printf("Later (in %d weeks):", diff));
            }
        }
    } else
        gtk_label_set_label((GtkLabel *) label, "Unplanned:");
    g_date_time_unref(now);
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
filter_func(GObject *item,
            gpointer user_data) {
    if (!PLANIFIED_IS_TASK(item))
        return FALSE;
    PlanifiedTask *task = PLANIFIED_TASK(item);
    if (planified_task_get_is_complete(task))
        return FALSE;
    gchar * date_type;
    GDateTime *sched = planified_task_get_most_relevant_date(task, &date_type);
    if (sched == NULL)
        return FALSE;
    GDateTime *now = g_date_time_new_now_local();
    int week_diff = g_date_time_get_week_of_year(sched) - g_date_time_get_week_of_year(now);
    g_date_time_unref(now);
    return (week_diff <= 1);
}

static gint
compare_week_func(gconstpointer a,
                  gconstpointer b,
                  gpointer user_data) {
    if (!PLANIFIED_IS_TASK(a) || !PLANIFIED_IS_TASK(b))
        return 0;
    PlanifiedTask *task_a = PLANIFIED_TASK(a);
    PlanifiedTask *task_b = PLANIFIED_TASK(b);
    gchar * date_type_a;
    gchar * date_type_b;
    GDateTime *a_sched = planified_task_get_most_relevant_date(task_a,&date_type_a);
    GDateTime *b_sched = planified_task_get_most_relevant_date(task_b,&date_type_b);
    GDateTime *now = g_date_time_new_now_local();


    gint y_diff = g_date_time_get_year(a_sched) - g_date_time_get_year(b_sched);
    gint w_diff = g_date_time_get_week_of_year(a_sched) - g_date_time_get_week_of_year(b_sched);

    gint res;
    // If both are overdue, bunch them together
    if ((g_date_time_difference(a_sched, now) < 0) &&
        (g_date_time_difference(b_sched, now) < 0) &&
        !IS_SAME_DAY(a_sched, now) && !IS_SAME_DAY(b_sched, now))
        res = 0;
    else if (y_diff != 0)
        res = y_diff;
    else if (IS_SAME_DAY(a_sched, now) != IS_SAME_DAY(b_sched, now))
        res = IS_SAME_DAY(a_sched, now) ? -1 : 1;
    else
        res = w_diff;

    g_date_time_unref(now);
    return res;

}


static gint
compare_day_func(gconstpointer a,
                 gconstpointer b,
                 gpointer user_data) {
    if (!PLANIFIED_IS_TASK(a) || !PLANIFIED_IS_TASK(b))
        return 0;
    PlanifiedTask *task_a = PLANIFIED_TASK(a);
    PlanifiedTask *task_b = PLANIFIED_TASK(b);
    GDateTime *b_sched = (planified_task_get_schedule(task_b));
    GDateTime *a_sched = (planified_task_get_schedule(task_a));
    gint diff = (gint) g_date_time_difference(a_sched, b_sched) % INT_MAX;
    return diff;
}

void
planified_upcoming_list_setup(PlanifiedUpcomingList *self) {
    GtkFilter *filter = (GtkFilter *) gtk_custom_filter_new((GtkCustomFilterFunc) filter_func, NULL, NULL);

    GListModel *raw_model = (GListModel *) planified_app_get_tasks(get_planified_app((GtkWidget *) self));
    GListModel *filtered_model = (GListModel *) gtk_filter_list_model_new(raw_model, filter);
    GtkSorter *week_sorter = (GtkSorter *) gtk_custom_sorter_new((GCompareDataFunc) compare_week_func, NULL, NULL);
    GtkSorter *day_sorter = (GtkSorter *) gtk_custom_sorter_new((GCompareDataFunc) compare_day_func, NULL, NULL);
    GListModel *sorted_model = (GListModel *) gtk_sort_list_model_new(filtered_model, NULL);
    gtk_sort_list_model_set_section_sorter((GtkSortListModel *) sorted_model, week_sorter);
    gtk_sort_list_model_set_sorter((GtkSortListModel *) sorted_model, day_sorter);

    self->model = (GListModel *) gtk_no_selection_new(sorted_model);
    gtk_list_view_set_model((GtkListView *) self->upcoming_list, (GtkSelectionModel *) self->model);

    gtk_scrolled_window_set_vadjustment((GtkScrolledWindow *) self->scrolled_win, 0);
}

static void
planified_upcoming_list_init(PlanifiedUpcomingList *self) {
    GtkWidget *title = gtk_label_new("Upcoming:");
    gtk_label_set_xalign((GtkLabel *) title, 0);

    gtk_widget_add_css_class(title, "title_text");
    gtk_box_append((GtkBox *) self, title);
    gtk_orientable_set_orientation((GtkOrientable *) self, GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_vexpand((GtkWidget *) self, TRUE);
    gtk_widget_set_valign((GtkWidget *) self, GTK_ALIGN_FILL);

    GtkListItemFactory *item_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (item_factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect (item_factory, "bind", G_CALLBACK(bind_list_item), NULL);
    g_signal_connect (item_factory, "unbind", G_CALLBACK(unbind_list_item), NULL);
    g_signal_connect (item_factory, "teardown", G_CALLBACK(teardown_list_item), NULL);
    GtkListItemFactory *header_factory = gtk_signal_list_item_factory_new();
    g_signal_connect (header_factory, "setup", G_CALLBACK(setup_list_header), NULL);
    g_signal_connect (header_factory, "bind", G_CALLBACK(bind_list_header), NULL);
    g_signal_connect (header_factory, "unbind", G_CALLBACK(unbind_list_header), NULL);
    g_signal_connect (header_factory, "teardown", G_CALLBACK(teardown_list_header), NULL);

    self->upcoming_list = gtk_list_view_new(NULL, item_factory);
    gtk_widget_add_css_class(self->upcoming_list, "no_selection");

    gtk_list_view_set_header_factory((GtkListView *) self->upcoming_list, header_factory);
    gtk_scrollable_set_vscroll_policy((GtkScrollable *) self->upcoming_list, GTK_SCROLL_NATURAL);

    GtkWidget *sw = gtk_scrolled_window_new();

    gtk_widget_set_hexpand(sw, TRUE);
    gtk_widget_set_vexpand(sw, TRUE);
    gtk_widget_set_halign(sw, GTK_ALIGN_FILL);
    gtk_widget_set_valign(sw, GTK_ALIGN_FILL);
    gtk_scrolled_window_set_child((GtkScrolledWindow *) sw, self->upcoming_list);
    gtk_scrolled_window_set_propagate_natural_height((GtkScrolledWindow *) sw, TRUE);

    gtk_widget_add_css_class(sw, "hide_overshoot");
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) sw, GTK_POLICY_EXTERNAL, GTK_POLICY_EXTERNAL);
    gtk_scrolled_window_set_kinetic_scrolling((GtkScrolledWindow *) sw, TRUE);
    gtk_box_append((GtkBox *) self, sw);
    self->scrolled_win = sw;
}

static void
planified_upcoming_list_class_init(PlanifiedUpcomingListClass *class) {
    GObjectClass *object_class = G_OBJECT_CLASS (class);
}

PlanifiedUpcomingList *
planified_upcoming_list_new() {
    PlanifiedUpcomingList *self = g_object_new(PLANIFIED_UPCOMING_LIST_TYPE, NULL);
    return self;
}
