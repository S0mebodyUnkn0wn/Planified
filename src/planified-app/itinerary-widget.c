#include <gtk/gtk.h>
#include "app.h"
#include "widgets.h"
#include "storage.h"

const int ROW_RESOLUTION = 30;
const int PLANNING_START_HOUR = 9;
const int PLANNING_END_HOUR = 24;

typedef enum {
    OPEN_DATE_CHANGED = 1,
    N_SIGNALS
} PlanifiedItineraryWidgetSignal;

static guint itinerary_signals[N_SIGNALS];

struct _PlanifiedItineraryWidget {
    GtkGrid parent;

    int width;
    GDateTime *open_on;
};

G_DEFINE_TYPE(PlanifiedItineraryWidget, planified_itinerary_widget, GTK_TYPE_GRID)


//TODO: Fix segfault
static void
insert(PlanifiedItineraryWidget *self, GtkWidget *item, int col, int row, int width, int height) {
    int insert_col = col;
    for (int i = row; i < row + height; i++) {
        GtkWidget *current_occupant = gtk_grid_get_child_at(GTK_GRID(self), insert_col, i);
        while (PLANIFIED_IS_ITINERARY_WIDGET_ENTRY(current_occupant)) {
            insert_col++;
            if (insert_col + 1 > self->width) {
                self->width = insert_col + 1;
            }
            current_occupant = gtk_grid_get_child_at(GTK_GRID(self), insert_col, i);
        }
//        if (PLANIFIED_IS_ITINERARY_WIDGET_ENTRY(current_occupant)) {

//            PlanifiedTask *occupant_task = planified_itinerary_widget_entry_get_task(
//                    PLANIFIED_ITINERARY_WIDGET_ENTRY(current_occupant));
//            gtk_grid_remove(GTK_GRID(self), current_occupant);
//            if (occupant_task != planified_itinerary_widget_entry_get_task(PLANIFIED_ITINERARY_WIDGET_ENTRY(item))) {
//                planified_task_set_schedule(occupant_task, -1);
//                database_update_task(get_handle(GTK_WIDGET(self)), occupant_task);
//            }
//        }
    }
    gtk_grid_attach(GTK_GRID(self), item, insert_col, row, width, height);
}

PlanifiedItineraryWidgetEntry *
find_entry_by_task(PlanifiedItineraryWidget *self, PlanifiedTask *task) {
    int i = 0;
    while (gtk_grid_get_child_at(GTK_GRID(self), 0, i) != NULL) {
        for (int j = 1; j < self->width; j++) {
            GtkWidget *item = gtk_grid_get_child_at(GTK_GRID(self), j, i);
            if (PLANIFIED_IS_ITINERARY_WIDGET_ENTRY(item)) {
                if (planified_itinerary_widget_entry_get_task(PLANIFIED_ITINERARY_WIDGET_ENTRY(item)) == task) {
                    return PLANIFIED_ITINERARY_WIDGET_ENTRY(item);
                }
            }
        }
        i++;
    }
    return NULL;
}

static gboolean
on_drop(GtkDropTarget *target,
        const GValue *value,
        double x,
        double y,
        gpointer data) {

    PlanifiedItineraryWidget *self = data;
    g_assert(PLANIFIED_IS_ITINERARY_WIDGET(self));

    if (G_VALUE_HOLDS(value, PLANIFIED_TASK_TYPE)) {
        PlanifiedTask *task = g_value_get_object(value);
        g_assert(PLANIFIED_IS_TASK(task));
        // Grabs the time label in the row over which the drop was performed
        GtkWidget *dropped_over_label = gtk_widget_pick(GTK_WIDGET(self), 30, y, GTK_PICK_DEFAULT);
        //TODO: Fix crutch above                                    ^
        if (dropped_over_label != NULL && gtk_widget_is_ancestor(dropped_over_label, GTK_WIDGET(self))) {
            int row = -1;
            gtk_grid_query_child(GTK_GRID(self), dropped_over_label, NULL, &row, NULL, NULL);
            if (row == -1) return FALSE;
            dropped_over_label = gtk_grid_get_child_at(GTK_GRID(self), 0, row);
            const gchar *selected_time = gtk_label_get_label(GTK_LABEL(dropped_over_label));
            GString *iso_datetime = g_string_new(g_date_time_format(self->open_on, "%F"));
            g_string_append(iso_datetime, "T");
            g_string_append(iso_datetime, selected_time);
            g_string_append(iso_datetime, ":00");
            GTimeZone *tz = g_time_zone_new_local();
            GDateTime *selected_date_time = g_date_time_new_from_iso8601(iso_datetime->str, tz);
            g_assert(selected_date_time != NULL);
            planified_task_set_schedule(task, selected_date_time);
            g_time_zone_unref(tz);
            g_string_free(iso_datetime, TRUE);
            g_print("Updating task...\n");
            database_update_task(get_handle(GTK_WIDGET(self)), task, 0);
            g_print("Wrote task to database\n");

            return TRUE;
        } else return FALSE;
    } else return FALSE;
}

void
planified_itinerary_widget_reload_data(PlanifiedItineraryWidget *self);

void
planified_itinerary_widget_refresh_data(GObject *_app, int operation, PlanifiedTask *updated_task, gpointer *_self) {
    PlanifiedItineraryWidget *self = PLANIFIED_ITINERARY_WIDGET(_self);
    for (int i = 1; i < self->width; i++) {
        gtk_grid_remove_column(GTK_GRID(self), 1);
    }
    self->width = 2;
    updated_task = nullptr;
//    if (operation == SQLITE_DELETE) {
//
//    };
    PlanifiedApp *app = PLANIFIED_APP(_app);
    GListStore *tasks;
    if (updated_task == NULL) {
        tasks = planified_app_get_tasks(app);
    } else {
        tasks = g_list_store_new(PLANIFIED_TASK_TYPE);
        g_list_store_append(tasks, updated_task);
    }
    for (guint i = 0; i < g_list_model_get_n_items((GListModel *) tasks); i++) {
        PlanifiedTask *task = g_list_model_get_item((GListModel *) tasks, i);
        GDateTime *start_time = planified_task_get_schedule(task);
        PlanifiedItineraryWidgetEntry *entry;
        entry = find_entry_by_task(self, task);
        if (start_time == NULL ||
            strcmp(g_date_time_format(start_time, "%Y-%m-%d"), g_date_time_format(self->open_on, "%Y-%m-%d")) != 0) {
            if (entry != NULL)
                gtk_grid_remove(GTK_GRID(self), GTK_WIDGET(entry));
        } else {
            int row = (60 / ROW_RESOLUTION) * (g_date_time_get_hour(start_time) - PLANNING_START_HOUR) +
                      (g_date_time_get_minute(start_time) / ROW_RESOLUTION);

            if (entry == NULL)
                entry = planified_itinerary_widget_entry_new(task, DATETIME_TO_UNIX_S(start_time));
            else {
                g_object_ref(entry);
                gtk_grid_remove(GTK_GRID(self), GTK_WIDGET(entry));
            }
            int height = (planified_task_get_timereq(task) + ROW_RESOLUTION - 1) / ROW_RESOLUTION;
            if (height <= 0)
                height = 1;
            insert(self, GTK_WIDGET(entry), 1, row, 1, height);
        }
        g_object_unref(task);
    }
    if (updated_task != NULL) {
        g_list_store_remove_all(tasks);
        g_object_unref(tasks);
    }

}


/*
 * The instance assumes ownership of `date`
 */
void
planified_itinerary_widget_set_selected_date(PlanifiedItineraryWidget *self, GDateTime *date) {
    g_date_time_unref(self->open_on);
    self->open_on = date;
    g_signal_emit(self, itinerary_signals[OPEN_DATE_CHANGED], 0);
}

void
planified_itinerary_widget_shift_selected_date(PlanifiedItineraryWidget *self, int shift_by) {
    GDateTime *t = g_date_time_add_days(self->open_on, shift_by);
    g_date_time_unref(self->open_on);
    self->open_on = t;
    g_signal_emit(self, itinerary_signals[OPEN_DATE_CHANGED], 0);
}

void
planified_itinerary_widget_go_to_next_day(PlanifiedItineraryWidget *self) {
    planified_itinerary_widget_shift_selected_date(self, 1);
}

void
planified_itinerary_widget_go_to_prev_day(PlanifiedItineraryWidget *self) {
    planified_itinerary_widget_shift_selected_date(self, -1);
}

GDateTime *
planified_itinerary_widget_get_selected_date(PlanifiedItineraryWidget *self) {
    return self->open_on;
}

static void
on_swipe(GtkGestureSwipe *swipe,
         gdouble velocity_x,
         gdouble velocity_y,
         gpointer user_data) {
    PlanifiedItineraryWidget *self = PLANIFIED_ITINERARY_WIDGET(user_data);
    if (((velocity_x) > 200 || (velocity_x < -200)) && (velocity_x * velocity_x > velocity_y * velocity_y)) {
        if (velocity_x < 0)
            planified_itinerary_widget_go_to_next_day(self);
        else
            planified_itinerary_widget_go_to_prev_day(self);
    }
}

static void
planified_itinerary_widget_init(PlanifiedItineraryWidget *widget) {
    widget->width = 2;
    GDateTime *local_time = g_date_time_new_now_local();
    widget->open_on = g_date_time_new_local(g_date_time_get_year(local_time),
                                            g_date_time_get_month(local_time),
                                            g_date_time_get_day_of_month(local_time),
                                            0, 0, 0);
    g_date_time_unref(local_time);

    // TODO: Adapt to work with custom values set by gsettings
    int row = 0;
    for (int i = PLANNING_START_HOUR; i < PLANNING_END_HOUR; i++) {
        for (int j = 0; j < 60; j += ROW_RESOLUTION) {
            char buf[6];
            sprintf(buf, "%.2d:%.2d", i, j);
            GtkWidget *label = gtk_label_new(buf);
            gtk_widget_add_css_class(label, "itinerary_time_label");
            if (j == 0)
                gtk_widget_add_css_class(label, "itinerary_time_label_hourly");
            gtk_widget_set_margin_start(label, 15);
            gtk_widget_set_margin_end(label, 15);
            gtk_widget_set_size_request(label, -1, 30);
            gtk_widget_set_valign(label, GTK_ALIGN_FILL);
            gtk_label_set_yalign(GTK_LABEL(label), 0);
            gtk_grid_attach(GTK_GRID(widget), label, 0, row, 1, 1);

            row++;
        }
    }

    GtkDropTarget *drop_target = gtk_drop_target_new(PLANIFIED_TASK_TYPE, GDK_ACTION_MOVE);
    GtkGesture *swipe = gtk_gesture_swipe_new();

//    g_signal_connect(swipe, "swipe", G_CALLBACK(on_swipe), widget);
    g_signal_connect(drop_target, "drop", G_CALLBACK(on_drop), widget);
    g_signal_connect_swapped(widget, "open-date-changed", G_CALLBACK(planified_itinerary_widget_reload_data), widget);

    gtk_widget_add_controller(GTK_WIDGET(widget), GTK_EVENT_CONTROLLER(drop_target));
    gtk_widget_add_controller(GTK_WIDGET(widget), GTK_EVENT_CONTROLLER(swipe));


}

static void
planified_itinerary_widget_class_init(PlanifiedItineraryWidgetClass *class) {
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    itinerary_signals[OPEN_DATE_CHANGED] =
            g_signal_newv("open-date-changed",
                          G_TYPE_FROM_CLASS (object_class),
                          G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                          NULL /* closure */,
                          NULL /* accumulator */,
                          NULL /* accumulator data */,
                          NULL /* C marshaller */,
                          G_TYPE_NONE /* return_type */,
                          0     /* n_params */,
                          NULL  /* param_types */);

}

void planified_itinerary_widget_reload_data(PlanifiedItineraryWidget *self) {
    for (int i = 1; i < self->width; i++) {
        gtk_grid_remove_column(GTK_GRID(self), 1);
    }
    self->width = 2;
    planified_itinerary_widget_refresh_data((GObject *) get_planified_app(GTK_WIDGET(self)), SQLITE_INSERT, NULL,
                                            (gpointer) self);
}

void
planified_itinerary_widget_setup(PlanifiedItineraryWidget *self) {
    g_signal_connect(get_planified_app(GTK_WIDGET(self)),
                     "task-array-updated", (GCallback) planified_itinerary_widget_refresh_data, self);
}

PlanifiedItineraryWidget *
planified_itinerary_widget_new() {
    PlanifiedItineraryWidget *self = g_object_new(PLANIFIED_ITINERARY_WIDGET_TYPE, NULL);
    return self;
}
