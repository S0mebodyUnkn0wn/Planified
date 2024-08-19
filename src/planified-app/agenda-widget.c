#include <gtk/gtk.h>
#include "app.h"
#include "widgets.h"
#include "storage.h"

typedef enum {
    OPEN_DATE_CHANGED = 1,
    N_SIGNALS
} PlanifiedAgendaWidgetProperties;

static guint agenda_signals[N_SIGNALS];

struct _PlanifiedAgendaWidget {
    GtkBox parent;

    GDateTime *open_on;
    GtkWidget *agenda_list;

};

G_DEFINE_TYPE(PlanifiedAgendaWidget, planified_agenda_widget, GTK_TYPE_BOX)

void
planified_agenda_widget_shift_open_day(PlanifiedAgendaWidget *self, int shift_by) {
    GDateTime *t = g_date_time_add_days(self->open_on, shift_by);
    g_free(self->open_on);
    self->open_on = t;
    g_signal_emit(self, agenda_signals[OPEN_DATE_CHANGED], 0);
}

void
planified_agenda_widget_go_to_next_day(PlanifiedAgendaWidget *self) {
    planified_agenda_widget_shift_open_day(self, 1);
}

void
planified_agenda_widget_go_to_prev_day(PlanifiedAgendaWidget *self) {
    planified_agenda_widget_shift_open_day(self, -1);
}

GDateTime *
planified_agenda_widget_get_selected_date(PlanifiedAgendaWidget *self) {
    return self->open_on;
}

void
planified_agenda_widget_refresh_data(PlanifiedAgendaWidget *self){
    gtk_list_box_remove_all(GTK_LIST_BOX(self->agenda_list));
    sqlite3_stmt *get_tasks_stmt;
    sqlite3 *handle = get_handle(GTK_WIDGET(self));
    g_assert(handle != NULL);
    sqlite3_prepare_v2(handle,
                       "select *, rowid from tasks where date(deadline, 'unixepoch','localtime')==date(?, 'unixepoch','localtime');",
                       -1,
                       &get_tasks_stmt,
                       NULL);
    sqlite3_bind_int64(get_tasks_stmt, 1, g_date_time_to_unix(self->open_on));
    int status;
    while ((status = sqlite3_step(get_tasks_stmt)) == SQLITE_ROW) {
        PlanifiedTask *task = planified_task_new_from_sqlite(get_tasks_stmt);
        gtk_list_box_insert(GTK_LIST_BOX(self->agenda_list), GTK_WIDGET(planified_task_widget_new(task)),-1);
    }
    if (status != SQLITE_DONE) {
        g_printerr("UPDATE FAILED WITH STATUS: %d", status);
    }
    sqlite3_finalize(get_tasks_stmt);
}

static void
planified_agenda_widget_init(PlanifiedAgendaWidget *self){
    self->agenda_list = gtk_list_box_new();
    gtk_box_append(GTK_BOX(self),self->agenda_list);
    GDateTime *local_time = g_date_time_new_now_local();
    self->open_on = g_date_time_new_local(g_date_time_get_year(local_time),
                                            g_date_time_get_month(local_time),
                                            g_date_time_get_day_of_month(local_time),
                                            0, 0, 0);
    g_date_time_unref(local_time);

    g_signal_connect_swapped(self,"open-date-changed",G_CALLBACK(planified_agenda_widget_refresh_data),self);

}

static void
planified_agenda_widget_class_init(PlanifiedAgendaWidgetClass *class){
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    agenda_signals[OPEN_DATE_CHANGED] =
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

PlanifiedAgendaWidget *
planified_agenda_widget_new(){
    PlanifiedAgendaWidget *self = g_object_new(PLANIFIED_AGENDA_WIDGET_TYPE,NULL);
    return self;
}
